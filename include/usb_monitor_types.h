#pragma once

#include <concepts>
#include <cstdint>
#include <functional>
#include <string_view>

namespace usb_monitor {

// USB设备状态枚举（C++20强类型枚举）
enum class UsbState : std::uint8_t {
    Removed,      // U盘拔出
    Inserted,     // U盘插入
    UpdateReady   // 更新U盘（有update文件夹且不为空）
};

// 日志级别枚举
enum class LogLevel : std::uint8_t {
    Trace,
    Debug,
    Info,
    Warning,
    Error
};

// C++20 Concept: USB事件回调函数约束
// 要求函数可调用，接受UsbState和string_view参数，返回void
template<typename F>
concept UsbEventCallback = std::invocable<F, UsbState, std::string_view> &&
    std::same_as<std::invoke_result_t<F, UsbState, std::string_view>, void>;

// C++20 Concept: 日志回调函数约束
// 要求函数可调用，接受LogLevel和string_view参数，返回void
template<typename F>
concept LogCallback = std::invocable<F, LogLevel, std::string_view> &&
    std::same_as<std::invoke_result_t<F, LogLevel, std::string_view>, void>;

} // namespace usb_monitor
