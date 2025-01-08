#ifndef USBMONITOR_H
#define USBMONITOR_H

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
    explicit USBMonitor(std::function<void(UsbState, std::string)> state);
    ~USBMonitor();
    void startMonitoring();

    static bool isDirectoryNotEmpty(const std::filesystem::path& path) {
        // 检查路径是否存在且为目录
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
            // 如果是目录，检查是否为空
            auto it = std::filesystem::directory_iterator(path);
            return it != std::filesystem::end(it); // 如果目录不为空，返回 true
        }
        return false;
    }
    static bool isUSBMounted(const std::string& path) {
        std::ifstream mounts("/proc/mounts");
        std::string line;
        while (std::getline(mounts, line)) {
            if (line.find(path) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

private:
    void monitorUSB();
    void findUSBMountPaths();

    std::set<std::string> currentUSBPaths; // 记录当前 USB 路径
    mutable std::mutex m;
    std::atomic_bool stop = false;
    std::function<void(UsbState,std::string)> USBstate; // 用于外部注入代码，发送插入与拔出的信号
    std::thread usbMonitorThread;
};

#endif // USBMONITOR_H
