
#include "macUSBMonitor.h"
#include <thread>
#include <iostream>
// #include <mutex>
#include <set>
// #include <chrono>
#include <filesystem>

using namespace std::chrono_literals;

namespace fs = std::filesystem;

void USBMonitor::startMonitoring() {
    stopFlag_ = false;
    monitorThread_ = std::thread(&USBMonitor::monitorVolumes, this);
}

void USBMonitor::stop() {
    stopFlag_ = true;
    if (monitorThread_.joinable()) monitorThread_.join();
}


void USBMonitor::monitorVolumes() {
    std::set<std::string> currentVolumes;
    while (!stopFlag_) {
        std::this_thread::sleep_for(300ms);
        std::set<std::string> newVolumes;
        std::error_code ec;
        for (const auto& entry : fs::directory_iterator("/Volumes", ec)) {
            if (entry.is_symlink(ec)) continue; // 跳过软链接（如 Macintosh HD）
            if (entry.is_directory(ec)) {
                newVolumes.insert(entry.path().string());
            }
        }

        // 检查新插入
        for (const auto& vol : newVolumes) {
            if (currentVolumes.insert(vol).second) {
                bool is_update = isUpdateUSB(vol);
                UsbState state = is_update ? UsbState::UpdateReady : UsbState::Inserted;
                std::clog << "macOS USB 插入: " << vol << '\n';
                if (callback_) callback_(state, vol);
            }
        }
        // 检查拔出
        for (const auto& vol : currentVolumes) {
            if (newVolumes.find(vol) == newVolumes.end()) {
                std::clog << "macOS USB 拔出: " << vol << '\n';
                if (callback_) callback_(UsbState::Removed, vol);
            }
        }
        currentVolumes = std::move(newVolumes);
    }
}

bool USBMonitor::isUpdateUSB(const std::string& volPath) {
    std::error_code ec;
    fs::path updatePath = volPath + "/update";
    if (fs::exists(updatePath, ec) && fs::is_directory(updatePath, ec)) {
        auto it = fs::directory_iterator(updatePath, ec);
        if (it != fs::end(it)) {
            return true;
        }
    }
    return false;
}

