#include <USBMonitor.h>

int main(){
    USBMonitor monitor([](UsbState state, std::string path){
        switch (state) {
            case UsbState::Removed:
                std::clog << "USB Removed: " << path << std::endl;
                break;
            case UsbState::Inserted:
                std::clog << "USB Inserted: " << path << std::endl;
                break;
            case UsbState::UpdateReady:
                std::clog << "USB Update Ready: " << path << std::endl;
                break;
        }
    });
    monitor.startMonitoring();
    std::cin.get();
}