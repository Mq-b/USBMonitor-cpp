#pragma once

#include <set>
#include <filesystem>
#include <fstream>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <functional>
#include <string>
#include <iostream>

using namespace std::chrono_literals;  // NOLINT(clang-diagnostic-header-hygiene)

enum UsbState {
    Removed,    // U盘拔出
    Inserted,   // U盘插入
    UpdateReady // 是一个更新U盘（有 update 文件夹且不为空）
};

class USBMonitor {
public:
    USBMonitor(std::function<void(UsbState, std::string)> state)
        : USBstate{ std::move(state) } {
    }

    ~USBMonitor() {
        stop = true;
        if (usbMonitorThread.joinable())
            usbMonitorThread.join();
    }

    void startMonitoring() {
        usbMonitorThread = std::thread(&USBMonitor::monitorUSB, this);
    }

    static bool isDirectoryNotEmpty(const std::filesystem::path& path) {
        // 检查路径是否存在且为目录
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
            // 如果是目录，检查是否为空
            auto it = std::filesystem::directory_iterator(path);
            return it != std::filesystem::end(it); // 如果目录不为空，返回 true
        }
        return false;
    }
    static std::string escapePath(const std::string& path) {
        std::string result = path;
        size_t pos = 0;
        while ((pos = result.find(" ", pos)) != std::string::npos) {
            result.replace(pos, 1, "\\040");
            pos += 4; // 跳过替换后的 "\040"
        }
        return result;
    }

    static bool isUSBMounted(const std::string& path) {
        std::string escapedPath = escapePath(path);

        std::ifstream mounts("/proc/mounts");
        std::string line;
        while (std::getline(mounts, line)) {
            if (line.find(escapedPath) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

private:
    void monitorUSB() {
        while (!stop) {
            std::this_thread::sleep_for(100ms);
            findUSBMountPaths();
        }
    }
    void findUSBMountPaths() {
        constexpr static std::string_view usbMountPath = "/media";
        std::set<std::string> newUSBPaths;

        for (const auto& entry : std::filesystem::directory_iterator{ usbMountPath }) {
            if (entry.is_directory()) {
                std::string userPath = entry.path().string();

                for (const auto& usbEntry : std::filesystem::directory_iterator{ userPath }) {
                    if (usbEntry.is_directory()) {
                        newUSBPaths.insert(usbEntry.path().string());
                    }
                }
            }
        }
        // 检查新 USB 路径并发出插入信号
        for (const auto& usbPath : newUSBPaths) {
            if (std::lock_guard lc{ m }; currentUSBPaths.insert(usbPath).second) { // 如果是新路径，则插入并发送信号
                for(int i = 0; i < 5; i++){
                    if(isUSBMounted(usbPath))
                        break;
                    std::this_thread::sleep_for(300ms);
                }

                bool is_update = isDirectoryNotEmpty(usbPath + "/update");

                if (is_update) {
                    std::clog << "插入更新U盘：" << usbPath << '\n';
                    USBstate(UsbState::UpdateReady, usbPath);
                }
                else {
                    std::clog << "插入U盘：" << usbPath << '\n';
                    USBstate(UsbState::Inserted, usbPath);
                }
            }
        }

        // 检查拔出路径并发出拔出信号
        for (const auto& usbPath : currentUSBPaths) {
            if (newUSBPaths.find(usbPath) == newUSBPaths.end()) { // 如果当前路径不在新路径中
                std::clog << "拔出U盘：" << usbPath << '\n';
                USBstate(UsbState::Removed, usbPath); // 出U盘，发送 false 与拔出的设备路径
            }
        }

        std::lock_guard lc{ m };
        currentUSBPaths = std::move(newUSBPaths); // 更新当前 USB 路径
    }

    std::set<std::string> currentUSBPaths; // 记录当前 USB 路径
    mutable std::mutex m;
    std::atomic_bool stop = false;
    std::function<void(UsbState,std::string)> USBstate; // 用于外部注入代码，发送插入与拔出的信号
    std::thread usbMonitorThread;
};
