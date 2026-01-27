#pragma once

#include "usb_monitor_impl.h"

// 包含平台特定实现
#include "platform/win_usb_monitor_impl.h"
#include "platform/unix_usb_monitor_impl.h"

namespace usb_monitor {

// USBMonitor的内部实现类（在detail命名空间中）
class USBMonitor::Impl {
public:
    template<UsbEventCallback F>
    Impl(F&& callback, std::unique_ptr<ILogger> logger) {
        // 将用户回调包装成EventCallback类型
        auto cb = [callback = std::forward<F>(callback)](UsbState state, std::string_view path) {
            callback(state, path);
        };

        // 使用工厂函数创建平台特定实现
        impl_ = detail::create_platform_impl(std::move(cb), std::move(logger));
    }

    void start() {
        impl_->start();
    }

    void stop() {
        impl_->stop();
    }

    [[nodiscard]] bool is_running() const noexcept {
        return impl_->is_running();
    }

    void set_poll_interval(std::chrono::milliseconds interval) {
        impl_->set_poll_interval(interval);
    }

private:
    std::unique_ptr<detail::USBMonitorImplBase> impl_;
};

// 工厂函数实现（平台选择逻辑在这里）
namespace detail {

inline std::unique_ptr<USBMonitorImplBase> create_platform_impl(
    USBMonitorImplBase::EventCallback callback,
    std::unique_ptr<ILogger> logger)
{
#ifdef _WIN32
    return std::make_unique<WinUSBMonitorImpl>(std::move(callback), std::move(logger));
#elif defined(__linux__) || defined(__unix__)
    return std::make_unique<UnixUSBMonitorImpl>(std::move(callback), std::move(logger));
#else
    #error "Unsupported platform: only Windows and Unix/Linux are supported"
#endif
}

} // namespace detail

// USBMonitor成员函数的实现（必须在Impl定义之后）
template<UsbEventCallback F>
inline USBMonitor::USBMonitor(F&& callback, std::unique_ptr<ILogger> logger)
    : pimpl_(std::make_unique<Impl>(std::forward<F>(callback), std::move(logger)))
{
}

// 析构函数定义（必须在Impl完整定义之后）
inline USBMonitor::~USBMonitor() = default;

// 移动构造函数和移动赋值运算符
inline USBMonitor::USBMonitor(USBMonitor&&) noexcept = default;
inline USBMonitor& USBMonitor::operator=(USBMonitor&&) noexcept = default;

// 成员函数实现
inline void USBMonitor::start() {
    pimpl_->start();
}

inline void USBMonitor::stop() {
    pimpl_->stop();
}

inline bool USBMonitor::is_running() const noexcept {
    return pimpl_->is_running();
}

inline void USBMonitor::set_poll_interval(std::chrono::milliseconds interval) {
    pimpl_->set_poll_interval(interval);
}

} // namespace usb_monitor
