#include <usb_monitor.h>

using namespace usb_monitor;

int main(){
    USBMonitor monitor([](UsbState state, std::string_view path){
        switch (state) {
            case UsbState::Removed:
                std::clog << "USB Removed: " << path << '\n';
                break;
            case UsbState::Inserted:
                std::clog << "USB Inserted: " << path << '\n';
                break;
            case UsbState::UpdateReady:
                std::clog << "USB Update Ready: " << path << '\n';
                break;
        }
    });

    monitor.start();
    std::cin.get();

    return 0;
}