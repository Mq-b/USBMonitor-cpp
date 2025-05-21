#include <USBMonitor.h>

int main(){
    USBMonitor monitor([](UsbState state, std::string path){
        switch (state) {
            case UsbState::Removed:
                std::cout << "USB Removed: " << path << std::endl;
                break;
            case UsbState::Inserted:
                std::cout << "USB Inserted: " << path << std::endl;
                break;
            case UsbState::UpdateReady:
                std::cout << "USB Update Ready: " << path << std::endl;
                break;
        }
    });
    monitor.startMonitoring();
    std::cin.get();
}