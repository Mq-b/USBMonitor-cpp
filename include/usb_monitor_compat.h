#pragma once

// 向后兼容层：支持旧代码平滑迁移
// 此文件提供deprecated的类型别名，帮助用户逐步迁移到新API

#include "usb_monitor.h"

// 全局命名空间的deprecated别名（如果用户之前没有使用namespace）
[[deprecated("Please use usb_monitor::UsbState instead")]]
using UsbState = usb_monitor::UsbState;

[[deprecated("Please use usb_monitor::USBMonitor instead")]]
using USBMonitor = usb_monitor::USBMonitor;

// 为旧的枚举值提供兼容别名
namespace {
    [[deprecated("Please use usb_monitor::UsbState::Removed instead")]]
    constexpr auto Removed = usb_monitor::UsbState::Removed;

    [[deprecated("Please use usb_monitor::UsbState::Inserted instead")]]
    constexpr auto Inserted = usb_monitor::UsbState::Inserted;

    [[deprecated("Please use usb_monitor::UsbState::UpdateReady instead")]]
    constexpr auto UpdateReady = usb_monitor::UsbState::UpdateReady;
}

// 迁移说明：
// 1. 包含头文件: #include <USBMonitor.h> → #include <usb_monitor.h>
// 2. 使用命名空间: 添加 using namespace usb_monitor;
// 3. 枚举访问: UsbState::Removed → usb_monitor::UsbState::Removed
// 4. API调用: monitor.startMonitoring() → monitor.start()
// 5. 参数类型: std::string → std::string_view（在回调函数中）
