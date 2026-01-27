#include <usb_monitor.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>

using namespace usb_monitor;

// 获取当前时间字符串
std::string get_current_time() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

int main() {
    std::cout << "=== USB Monitor - 自定义日志示例 ===\n\n";

    // 创建自定义日志器（输出格式化的日志）
    auto logger = std::make_unique<CallbackLogger>(
        [](LogLevel level, std::string_view msg) {
            // 自定义日志格式：[时间] [级别] 消息
            std::cout << "[" << get_current_time() << "] "
                     << "[" << log_level_to_string(level) << "] "
                     << msg << '\n';
        }
    );

    // 使用自定义日志器创建监控器
    USBMonitor monitor(
        [](UsbState state, std::string_view path) {
            std::string emoji;
            std::string action;

            switch (state) {
                case UsbState::Removed:
                    emoji = "❌";
                    action = "已拔出";
                    break;
                case UsbState::Inserted:
                    emoji = "✅";
                    action = "已插入";
                    break;
                case UsbState::UpdateReady:
                    emoji = "🔄";
                    action = "更新盘就绪";
                    break;
            }

            std::cout << emoji << " USB " << action << ": " << path << '\n';
        },
        std::move(logger)
    );

    std::cout << "USB监控已启动（带自定义日志）\n";
    std::cout << "按Enter键退出...\n\n";

    monitor.start();
    std::cin.get();

    return 0;
}
