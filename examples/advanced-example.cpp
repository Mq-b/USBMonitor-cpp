#include <usb_monitor.h>
#include <iostream>
#include <chrono>
#include <thread>

using namespace usb_monitor;
using namespace std::chrono_literals;

int main() {
    std::cout << "=== USB Monitor - 高级功能示例 ===\n\n";

    USBMonitor monitor([](UsbState state, std::string_view path) {
        std::cout << "USB事件: ";
        switch (state) {
            case UsbState::Removed:
                std::cout << "拔出";
                break;
            case UsbState::Inserted:
                std::cout << "插入";
                break;
            case UsbState::UpdateReady:
                std::cout << "更新盘";
                break;
        }
        std::cout << " - " << path << '\n';
    });

    // 1. 设置自定义轮询间隔（默认100ms）
    std::cout << "设置轮询间隔为500ms\n";
    monitor.set_poll_interval(500ms);

    // 2. 启动监控
    std::cout << "启动监控...\n";
    monitor.start();

    // 3. 检查运行状态
    if (monitor.is_running()) {
        std::cout << "✓ 监控器正在运行\n\n";
    }

    std::cout << "监控运行5秒后自动停止...\n";
    std::this_thread::sleep_for(5s);

    // 4. 手动停止
    std::cout << "\n正在停止监控...\n";
    monitor.stop();

    // 5. 再次检查状态
    if (!monitor.is_running()) {
        std::cout << "✓ 监控器已停止\n";
    }

    std::cout << "\n按Enter键退出...\n";
    std::cin.get();

    return 0;
}
