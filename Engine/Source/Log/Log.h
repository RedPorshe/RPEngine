#pragma once
#include <string>
#include <string_view>
#include <memory>
#include <source_location>
#include <concepts>
#include <format>

#include "Core/Utility.h"

namespace RPE
{

enum class LogVerbosity : uint8_t
{
    NoLogging = 0,
    Display,
    Warning,
    Error,
    Log,
    Fatal
};

struct LogCategory
{
    explicit LogCategory(const std::string& name) : m_name(name) {}
    std::string name() const { return m_name; }

private:
    const std::string m_name;
};

class Log final : public NonCopyable
{
public:
    static Log& getInstance()
    {
        static Log instance;
        return instance;
    }

    // !! Do not use the direct call; it's unsafe. Use macro version with static checks !!
    void log(const LogCategory& category,  //
        LogVerbosity verbosity,            //
        const std::string& message,        //
        bool showLocation = false,         //
        const std::source_location location = std::source_location::current()) const;

private:
    Log();
    ~Log();

    class Impl;
    std::unique_ptr<Impl> m_pImpl;
};

constexpr LogVerbosity c_minVerbosity = LogVerbosity::Display;
constexpr LogVerbosity c_maxVerbosity = LogVerbosity::Fatal;

// concepts
template <typename T>
concept ValidLogCategory = std::constructible_from<LogCategory, T>;

template <typename T>
concept LoggableMessage = std::convertible_to<T, std::string> || std::convertible_to<T, std::string_view>;

template <LogVerbosity V>
concept ValidVerbosityLevel = V == LogVerbosity::NoLogging   //
                              || V == LogVerbosity::Display  //
                              || V == LogVerbosity::Warning  //
                              || V == LogVerbosity::Error    //
                              || V == LogVerbosity::Log      //
                              || V == LogVerbosity::Fatal;

}  // namespace RPE

#define DEFINE_LOG_CATEGORY_STATIC(logName)   \
    namespace                                 \
    {                                         \
    const RPE::LogCategory logName(#logName); \
    }

#define RP_LOG_IMPL(categoryName, verbosity, showLocation, formatStr, ...)                                                              \
    do                                                                                                                                  \
    {                                                                                                                                   \
        if constexpr (RPE::LogVerbosity::verbosity >= RPE::c_minVerbosity && RPE::LogVerbosity::verbosity <= RPE::c_maxVerbosity)       \
        {                                                                                                                               \
            static_assert(RPE::ValidVerbosityLevel<RPE::LogVerbosity::verbosity>,                                                       \
                "Verbosity must be one of: NoLogging, Display, Warning, Error, Log, Fatal");                                            \
            static_assert(RPE::ValidLogCategory<decltype(categoryName)>, "Category must be of type LogCategory");                       \
            static_assert(RPE::LoggableMessage<decltype(formatStr)>, "Message must be convertible to std::string or std::string_view"); \
            RPE::Log::getInstance().log(                                                                                                \
                categoryName, RPE::LogVerbosity::verbosity, std::format(formatStr __VA_OPT__(, ) __VA_ARGS__), showLocation);           \
        }                                                                                                                               \
    } while (0)

#define RP_LOG(categoryName, verbosity, formatStr, ...) RP_LOG_IMPL(categoryName, verbosity, false, formatStr, ##__VA_ARGS__)
#define RP_LOG_DEBUG(categoryName, verbosity, formatStr, ...) RP_LOG_IMPL(categoryName, verbosity, true, formatStr, ##__VA_ARGS__)