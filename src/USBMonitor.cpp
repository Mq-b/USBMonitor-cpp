#include "USBMonitor.h"

USBMonitor::USBMonitor(std::function<void(UsbState, std::string)> state)
    : USBstate{std::move(state)} {}

USBMonitor::~USBMonitor() {
    stop = true;
    if (usbMonitorThread.joinable())
        usbMonitorThread.join();
}

void USBMonitor::startMonitoring() {
    usbMonitorThread = std::thread(&USBMonitor::monitorUSB, this);
}

void USBMonitor::monitorUSB() {
    while (!stop) {
        std::this_thread::sleep_for(100ms);
        findUSBMountPaths();
    }
}

void USBMonitor::findUSBMountPaths() {
    constexpr static std::string_view usbMountPath = "/media";
    std::set<std::string> newUSBPaths;

    for (const auto& entry : std::filesystem::directory_iterator{usbMountPath}) {
        if (entry.is_directory()) {
            std::string userPath = entry.path().string();

            for (const auto& usbEntry : std::filesystem::directory_iterator{userPath}) {
                if (usbEntry.is_directory()) {
                    newUSBPaths.insert(usbEntry.path().string());
                }
            }
        }
    }
    // 检查新 USB 路径并发出插入信号
    for (const auto& usbPath : newUSBPaths) {
        if (std::lock_guard lc{m};currentUSBPaths.insert(usbPath).second) { // 如果是新路径，则插入并发送信号

            while (!isUSBMounted(usbPath)) {
                std::this_thread::sleep_for(300ms); // 延时300毫秒
            }

            bool is_update = isDirectoryNotEmpty(usbPath + "/update");

            if(is_update){
                std::clog<< "插入更新U盘：" << usbPath << '\n';
                USBstate(UsbState::UpdateReady,usbPath);
            }
            else{
                std::clog<< "插入U盘：" << usbPath << '\n';
                USBstate(UsbState::Inserted,usbPath);
            }
        }
    }

    // 检查拔出路径并发出拔出信号
    for (const auto& usbPath : currentUSBPaths) {
        if (newUSBPaths.find(usbPath) == newUSBPaths.end()) { // 如果当前路径不在新路径中
            std::clog<< "拔出U盘：" << usbPath << '\n';
            USBstate(UsbState::Removed,usbPath); // 出U盘，发送 false 与拔出的设备路径
        }
    }

    std::lock_guard lc{m};
    currentUSBPaths = std::move(newUSBPaths); // 更新当前 USB 路径
}
