#include <USBMonitor.h>
#include <iostream>
#include <future>  
#include <csignal> 

static std::promise<void> g_exitSignal;

void handleSigInt(int) {
    g_exitSignal.set_value();
}

int main() {
    std::signal(SIGINT, handleSigInt);

    USBMonitor mon([](UsbState s, std::string p){
        static constexpr const char* names[] = {"Removed","Inserted","UpdateReady"};
        std::printf("USB %s: %s\n", names[s], p.c_str());
    });

    mon.startMonitoring();

    std::cout << "Listening for USB events. Press Ctrl+C to quit...\n";

    g_exitSignal.get_future().wait();

    return 0;
}

