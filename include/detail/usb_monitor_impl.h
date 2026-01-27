#pragma once

#include "../usb_monitor_types.h"
#include "../usb_monitor_logger.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>

namespace usb_monitor::detail {

// 平台无关的USBMonitor实现基类
class USBMonitorImplBase {
public:
    using EventCallback = std::function<void(UsbState, std::string_view)>;

    virtual ~USBMonitorImplBase() = default;

    // 纯虚函数，由平台特定类实现
    virtual void start() = 0;
    virtual void stop() = 0;
    [[nodiscard]] virtual bool is_running() const noexcept = 0;

    // 设置轮询间隔
    virtual void set_poll_interval(std::chrono::milliseconds interval) {
        poll_interval_ = interval;
    }

protected:
    // 受保护的成员，供派生类使用
    EventCallback event_callback_;
    std::unique_ptr<ILogger> logger_;
    std::atomic<bool> running_{false};
    std::jthread monitor_thread_;  // C++20: 自动join和停止
    std::chrono::milliseconds poll_interval_{100};  // 默认100ms轮询
};

// 平台特定实现的工厂函数声明
// 实际实现在 usb_monitor_inline.h 中（根据平台条件编译）
std::unique_ptr<USBMonitorImplBase> create_platform_impl(
    USBMonitorImplBase::EventCallback callback,
    std::unique_ptr<ILogger> logger
);

} // namespace usb_monitor::detail
