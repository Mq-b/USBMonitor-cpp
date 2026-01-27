#pragma once

#include "usb_monitor_types.h"
#include "usb_monitor_logger.h"
#include <chrono>
#include <memory>

namespace usb_monitor {

// USB监控器主类（使用PImpl模式隐藏实现细节）
class USBMonitor {
public:
    // 构造函数：使用C++20 Concept约束回调类型
    // 默认使用DefaultLogger，用户可提供自定义Logger
    template<UsbEventCallback F>
    explicit USBMonitor(F&& callback,
                       std::unique_ptr<ILogger> logger = std::make_unique<DefaultLogger>());

    // 析构函数（在实现文件中定义，因为Impl是不完整类型）
    ~USBMonitor();

    // 移动语义支持（禁止拷贝）
    USBMonitor(USBMonitor&&) noexcept;
    USBMonitor& operator=(USBMonitor&&) noexcept;

    // 删除拷贝构造和拷贝赋值
    USBMonitor(const USBMonitor&) = delete;
    USBMonitor& operator=(const USBMonitor&) = delete;

    // 启动USB监控
    void start();

    // 停止USB监控
    void stop();

    // 查询监控状态
    [[nodiscard]] bool is_running() const noexcept;

    // 设置轮询间隔（默认100ms）
    void set_poll_interval(std::chrono::milliseconds interval);

private:
    // PImpl: 前向声明实现类（在detail命名空间中定义）
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace usb_monitor

// 在文件末尾包含实现（这是实现纯头文件库PImpl的关键技巧）
#include "detail/usb_monitor_inline.h"
