#pragma once

#include <windows.h>
#include <iostream>
#include <bitset>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <string>
#include <functional>
#include <set>
#include <filesystem>

using namespace std::chrono_literals; // NOLINT(clang-diagnostic-header-hygiene)
using namespace std::string_literals; // NOLINT(clang-diagnostic-header-hygiene)

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

private:
    bool isDirectoryNotEmpty(const std::filesystem::path& path) {
        // 检查路径是否存在且为目录
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
            // 如果是目录，检查是否为空
            auto it = std::filesystem::directory_iterator(path);
            return it != std::filesystem::end(it); // 如果目录不为空，返回 true
        }
        return false;
    }

    bool IsRemovableDrive(char letter) {
        char rootPath[] = "A:\\";
        rootPath[0] = letter;
        const UINT type = GetDriveTypeA(rootPath);
        return type == DRIVE_REMOVABLE; // 判断该盘符是否为可移动设备（U盘）
    }

    void monitorUSB() {
        while (!stop) {
            std::this_thread::sleep_for(100ms);
            findUSBMountPaths();
        }
    }
    void findUSBMountPaths() {
        std::set<std::string> newUSBPaths;
        const DWORD current = GetLogicalDrives(); // 返回一个位掩码，每一位代表一个盘符是否存在
        const std::bitset<26> currentDrives(current);

        for (int i = 0; i < 26; ++i) {
            if (const char driveLetter = 'A' + i; currentDrives[i] && IsRemovableDrive(driveLetter)) {
                newUSBPaths.insert(driveLetter + ":\\"s); // 记录当前已有磁盘路径
            }
        }

        // 比较新旧路径，如果有新的设备插入，则发送插入信号，反正有设备拔出则发送拔出信号
        // 检查新 USB 路径并发出插入信号
        for (const auto& usbPath : newUSBPaths) {
            if (std::lock_guard lc{ m }; currentUSBPaths.insert(usbPath).second) { // 如果是新路径，则插入并发送信号

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

    std::set<std::string> currentUSBPaths; // 记录当前已有磁盘路径
    mutable std::mutex m;
    std::atomic_bool stop = false;
    std::function<void(UsbState, std::string)> USBstate; // 用于外部注入代码，发送插入与拔出的信号
    std::thread usbMonitorThread;
};
