#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <atomic>
#include <thread>

enum class UsbState {
    Removed,
    Inserted,
    UpdateReady
};

class USBMonitor {
public:
    USBMonitor(std::function<void(UsbState, std::string)> state)
        : callback_(std::move(state)) {}

    ~USBMonitor() { stop(); }

    void startMonitoring();
    void stop();

private:
    std::atomic_bool stopFlag_{false};
    std::thread monitorThread_;
    void monitorVolumes();
    bool isUpdateUSB(const std::string& volPath);
    std::function<void(UsbState, std::string)> callback_;
};
