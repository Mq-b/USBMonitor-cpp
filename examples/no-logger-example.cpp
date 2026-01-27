#include <usb_monitor.h>
#include <iostream>

using namespace usb_monitor;

int main() {
    std::cout << "=== USB Monitor - 零日志开销示例 ===\n";
    std::cout << "使用NullLogger完全禁用内部日志（性能最优）\n\n";

    // 使用NullLogger完全禁用日志输出（零开销）
    // 适合生产环境或性能敏感场景
    USBMonitor monitor(
        [](UsbState state, std::string_view path) {
            // 只处理USB事件，无内部日志开销
            const char* state_str = nullptr;
            switch (state) {
                case UsbState::Removed:
                    state_str = "拔出";
                    break;
                case UsbState::Inserted:
                    state_str = "插入";
                    break;
                case UsbState::UpdateReady:
                    state_str = "更新盘";
                    break;
            }

            std::cout << "[用户回调] USB " << state_str << ": " << path << '\n';
        },
        std::make_unique<NullLogger>()  // 零开销日志
    );

    monitor.start();

    std::cout << "\nUSB监控已启动（零日志模式）\n";
    std::cout << "内部日志已禁用，仅显示用户回调输出\n";
    std::cout << "按Enter键退出...\n\n";

    std::cin.get();

    return 0;
}
