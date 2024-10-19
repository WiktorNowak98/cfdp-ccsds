#pragma once

#include <chrono>
#include <cstdint>
#include <format>
#include <mutex>
#include <thread>

#if defined(CFDP_LOG_LEVEL_TRACE)
#define LOG_TRACE
#endif

#if defined(LOG_TRACE) || defined(CFDP_LOG_LEVEL_DEBUG)
#define LOG_DEBUG
#endif

#if defined(LOG_DEBUG) || defined(CFDP_LOG_LEVEL_INFO)
#define LOG_INFO
#endif

#if defined(LOG_INFO) || defined(CFDP_LOG_LEVEL_WARN)
#define LOG_WARN
#endif

#if defined(LOG_WARN) || defined(CFDP_LOG_LEVEL_ERROR)
#define LOG_ERROR
#endif

namespace cfdp::runtime::logging
{
enum class LogLevel : uint8_t
{
    Trace = 0,
    Debug,
    Info,
    Warn,
    Error,
};

class Logger
{
  public:
    void log(LogLevel level, std::string msg) const noexcept;

  private:
    // NOTE: 14.10.2024 <@uncommon-nickname>
    // We need this mutex to be static, our entrypoint logging functions
    // use templates and each specialization has its own static logger
    // object. This way we will avoid log races.
    static inline std::mutex mutex{};

    [[nodiscard]] inline std::thread::id getThreadID() const noexcept
    {
        return std::this_thread::get_id();
    }

    [[nodiscard]] inline std::string getCurrentTimestamp() const noexcept
    {
        const auto now = std::chrono::system_clock::now();
        return std::format("{:%d-%m-%Y %H:%M:%OS}", now);
    }
};

template <class... Args>
constexpr static void log(LogLevel level, std::format_string<Args...> msg, Args&&... args) noexcept
{
    static auto logger = Logger{};

    auto log = std::vformat(msg.get(), std::make_format_args(args...));
    logger.log(level, std::move(log));
}

template <class... Args>
constexpr static void trace(std::format_string<Args...> msg, Args&&... args) noexcept
{
#if defined(LOG_TRACE)
    log(LogLevel::Trace, msg, std::forward<Args>(args)...);
#endif
}

template <class... Args>
constexpr static void debug(std::format_string<Args...> msg, Args&&... args) noexcept
{
#if defined(LOG_DEBUG)
    log(LogLevel::Debug, msg, std::forward<Args>(args)...);
#endif
}

template <class... Args>
constexpr static void info(std::format_string<Args...> msg, Args&&... args) noexcept
{
#if defined(LOG_INFO)
    log(LogLevel::Info, msg, std::forward<Args>(args)...);
#endif
}

template <class... Args>
constexpr static void warn(std::format_string<Args...> msg, Args&&... args) noexcept
{
#if defined(LOG_WARN)
    log(LogLevel::Warn, msg, std::forward<Args>(args)...);
#endif
}

template <class... Args>
constexpr static void error(std::format_string<Args...> msg, Args&&... args) noexcept
{
#if defined(LOG_ERROR)
    log(LogLevel::Error, msg, std::forward<Args>(args)...);
#endif
}
} // namespace cfdp::runtime::logging
