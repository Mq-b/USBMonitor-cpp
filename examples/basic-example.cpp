#include <usb_monitor.h>
#include <iostream>

using namespace usb_monitor;

int main() {
    std::cout << "=== USB Monitor - 基础示例（新API） ===\n";
    std::cout << "使用现代C++20特性：PImpl、Concepts、std::jthread\n\n";

    // 创建USB监控器，传入事件回调
    USBMonitor monitor([](UsbState state, std::string_view path) {
        switch (state) {
            case UsbState::Removed:
                std::cout << "✗ USB已拔出: " << path << '\n';
                break;
            case UsbState::Inserted:
                std::cout << "✓ USB已插入: " << path << '\n';
                break;
            case UsbState::UpdateReady:
                std::cout << "⚡ 更新U盘已插入: " << path << '\n';
                break;
        }
    });

    // 启动监控
    monitor.start();

    std::cout << "\nUSB监控已启动，按Enter键退出...\n";
    std::cin.get();

    // 析构时自动停止监控（RAII设计）
    std::cout << "正在停止监控...\n";
    return 0;
}
