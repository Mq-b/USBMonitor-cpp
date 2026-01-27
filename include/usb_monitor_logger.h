#pragma once

#include "usb_monitor_types.h"
#include <format>
#include <source_location>
#include <string>
#include <string_view>
#include <iostream>
#include <utility>

namespace usb_monitor {

// 抽象日志接口
class ILogger {
public:
    virtual ~ILogger() = default;

    // 主日志方法（支持C++20源码位置自动捕获）
    virtual void log(LogLevel level, std::string_view message,
                    std::source_location loc = std::source_location::current()) = 0;

    // 便捷方法（使用C++20 std::format）
    template<typename... Args>
    void trace(std::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::Trace, std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void debug(std::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::Debug, std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void info(std::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::Info, std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void warning(std::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::Warning, std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void error(std::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::Error, std::format(fmt, std::forward<Args>(args)...));
    }
};

// 日志级别转字符串（内联辅助函数）
inline constexpr std::string_view log_level_to_string(LogLevel level) noexcept {
    switch (level) {
        case LogLevel::Trace:   return "TRACE";
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

// 默认日志实现（输出到std::clog）
class DefaultLogger : public ILogger {
public:
    void log(LogLevel level, std::string_view message,
            std::source_location loc = std::source_location::current()) override {
        std::clog << std::format("[{}] {}\n",
            log_level_to_string(level),
            message);
    }
};

// 回调日志适配器（允许用户自定义日志处理）
class CallbackLogger : public ILogger {
public:
    // C++20 Concept约束的构造函数
    template<LogCallback F>
    explicit CallbackLogger(F&& callback)
        : callback_(std::forward<F>(callback)) {}

    void log(LogLevel level, std::string_view message,
            std::source_location loc = std::source_location::current()) override {
        if (callback_) {
            callback_(level, message);
        }
    }

private:
    std::function<void(LogLevel, std::string_view)> callback_;
};

// 空日志实现（零开销，适用于生产环境）
class NullLogger : public ILogger {
public:
    void log(LogLevel, std::string_view, std::source_location = {}) override {
        // 什么都不做，编译器会优化掉这个调用
    }
};

} // namespace usb_monitor
