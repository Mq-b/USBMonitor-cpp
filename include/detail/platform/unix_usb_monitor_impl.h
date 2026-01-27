#pragma once

// 平台宏检查：仅在Unix/Linux平台编译
#if defined(__linux__) || defined(__unix__)

#include "../usb_monitor_impl.h"
#include <filesystem>
#include <format>
#include <fstream>
#include <mutex>
#include <set>
#include <string>

namespace usb_monitor::detail {

// Unix/Linux平台USB监控实现
class UnixUSBMonitorImpl : public USBMonitorImplBase {
public:
    UnixUSBMonitorImpl(EventCallback callback, std::unique_ptr<ILogger> logger) {
        event_callback_ = std::move(callback);
        logger_ = std::move(logger);
    }

    void start() override {
        if (running_.exchange(true)) {
            logger_->warning("USB monitor already running");
            return;
        }

        // C++20 std::jthread: 自动传递stop_token，析构时自动join
        monitor_thread_ = std::jthread([this](std::stop_token stoken) {
            logger_->info("USB monitor started (Unix/Linux)");
            monitor_loop(stoken);
        });
    }

    void stop() override {
        if (!running_.exchange(false)) {
            return;
        }

        // jthread析构时会自动request_stop()和join()
        logger_->info("USB monitor stopped");
    }

    [[nodiscard]] bool is_running() const noexcept override {
        return running_.load(std::memory_order_relaxed);
    }

private:
    // 监控循环（使用C++20 stop_token协作式取消）
    void monitor_loop(std::stop_token stoken) {
        while (!stoken.stop_requested()) {
            scan_usb_devices();
            std::this_thread::sleep_for(poll_interval_);
        }
    }

    // 扫描USB设备
    void scan_usb_devices() {
        constexpr std::string_view usb_mount_path = "/media";
        std::set<std::string> new_paths;

        // 检查挂载目录是否存在
        if (!std::filesystem::exists(usb_mount_path)) {
            return;
        }

        try {
            // 遍历 /media 下的用户目录
            for (const auto& user_entry : std::filesystem::directory_iterator(usb_mount_path)) {
                if (!user_entry.is_directory()) {
                    continue;
                }

                // 遍历用户目录下的USB设备
                for (const auto& usb_entry : std::filesystem::directory_iterator(user_entry)) {
                    if (usb_entry.is_directory()) {
                        new_paths.insert(usb_entry.path().string());
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            logger_->error("Filesystem error while scanning USB: {}", e.what());
        }

        // 处理设备变化
        process_changes(new_paths);
    }

    // 处理USB设备变化
    void process_changes(const std::set<std::string>& new_paths) {
        std::lock_guard lock(mutex_);

        // 检测新插入的USB设备
        for (const auto& path : new_paths) {
            if (current_paths_.insert(path).second) {
                // 这是新插入的设备，等待挂载完成
                wait_for_mount(path);

                // 检查是否为更新U盘
                auto update_path = std::filesystem::path(path) / "update";
                bool is_update = is_directory_not_empty(update_path);

                UsbState state = is_update ? UsbState::UpdateReady : UsbState::Inserted;

                // 使用C++20 std::format记录日志
                if (is_update) {
                    logger_->info("USB update ready: {}", path);
                } else {
                    logger_->info("USB inserted: {}", path);
                }

                // 触发回调
                event_callback_(state, path);
            }
        }

        // 检测拔出的USB设备
        for (auto it = current_paths_.begin(); it != current_paths_.end();) {
            if (!new_paths.contains(*it)) {
                // 设备已拔出
                logger_->info("USB removed: {}", *it);
                event_callback_(UsbState::Removed, *it);
                it = current_paths_.erase(it);
            } else {
                ++it;
            }
        }
    }

    // 等待USB挂载完成
    void wait_for_mount(const std::string& path) {
        using namespace std::chrono_literals;

        // 最多等待3秒
        for (int i = 0; i < 10; ++i) {
            if (is_usb_mounted(path)) {
                return;
            }
            std::this_thread::sleep_for(300ms);
        }

        logger_->warning("USB mount timeout for: {}", path);
    }

    // 检查USB是否已挂载（通过读取/proc/mounts）
    [[nodiscard]] static bool is_usb_mounted(const std::string& path) {
        std::ifstream mounts("/proc/mounts");
        if (!mounts.is_open()) {
            return false;
        }

        std::string line;
        while (std::getline(mounts, line)) {
            if (line.contains(path)) {  // C++23: std::string::contains
                return true;
            }
        }

        return false;
    }

    // 检查目录是否非空
    [[nodiscard]] static bool is_directory_not_empty(const std::filesystem::path& path) {
        if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
            return false;
        }

        // 检查目录迭代器是否为空
        auto it = std::filesystem::directory_iterator(path);
        return it != std::filesystem::directory_iterator();
    }

    // 私有成员
    std::set<std::string> current_paths_;  // 当前USB路径缓存
    mutable std::mutex mutex_;             // 线程同步互斥锁
};

} // namespace usb_monitor::detail

#endif // __linux__ || __unix__
