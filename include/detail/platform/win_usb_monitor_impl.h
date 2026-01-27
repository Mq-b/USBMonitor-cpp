#pragma once

// 平台宏检查：仅在Windows平台编译
#ifdef _WIN32

#include "../usb_monitor_impl.h"
#include <windows.h>
#include <filesystem>
#include <format>
#include <mutex>
#include <set>
#include <string>

namespace usb_monitor::detail {

// Windows平台USB监控实现
class WinUSBMonitorImpl : public USBMonitorImplBase {
public:
    WinUSBMonitorImpl(EventCallback callback, std::unique_ptr<ILogger> logger) {
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
            logger_->info("USB monitor started (Windows)");
            monitor_loop(stoken);
        });
    }

    void stop() override {
        if (!running_.exchange(false)) {
            return;
        }

        // jthread析构时会自动request_stop()和join()
        // 但我们也可以显式调用（可选）
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
        std::set<std::string> new_paths;

        // 获取所有逻辑驱动器的位掩码
        const DWORD drives = GetLogicalDrives();

        // 遍历A-Z 26个驱动器字母
        for (int i = 0; i < 26; ++i) {
            if (drives & (1 << i)) {
                char letter = 'A' + static_cast<char>(i);
                if (is_removable_drive(letter)) {
                    // 使用C++20 std::format构建路径
                    new_paths.insert(std::format("{}:\\", letter));
                }
            }
        }

        // 处理设备变化
        process_changes(new_paths);
    }

    // 检查是否为可移动驱动器
    [[nodiscard]] bool is_removable_drive(char letter) const {
        char path[] = "A:\\";
        path[0] = letter;
        const UINT type = GetDriveTypeA(path);
        return type == DRIVE_REMOVABLE;
    }

    // 处理USB设备变化
    void process_changes(const std::set<std::string>& new_paths) {
        std::lock_guard lock(mutex_);

        // 检测新插入的USB设备
        for (const auto& path : new_paths) {
            if (current_paths_.insert(path).second) {
                // 这是新插入的设备
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

#endif // _WIN32
