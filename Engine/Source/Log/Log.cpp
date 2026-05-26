#include "Log.h"
#include <memory>
#include <unordered_map>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace RPE;

namespace
{
const std::unordered_map<LogVerbosity, spdlog::level::level_enum> c_verbosityMap =  //
    {{LogVerbosity::NoLogging, spdlog::level::off}, {LogVerbosity::Dysplay, spdlog::level::info},
        {LogVerbosity::Warning, spdlog::level::warn}, {LogVerbosity::Error, spdlog::level::err},
        {LogVerbosity::Fatal, spdlog::level::critical}};
}
//pImpl
class Log::Impl
    {
public:
    Impl ()
        {
        const auto consolesink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
       m_consoleLogger = std::make_unique<spdlog::logger>("console", consolesink);
            }
    void log ( LogVerbosity verbosity, const std::string & message ) const
        {
        if (verbosity == LogVerbosity::NoLogging) return;
            const auto spdLevel = c_verbosityMap.find(verbosity);
            if (spdLevel != c_verbosityMap.end())
            {
                m_consoleLogger->log(spdLevel->second, message.c_str());
            }
        }
    private:
            std::unique_ptr<spdlog::logger> m_consoleLogger;
    };


// interface

Log::Log() :m_pImpl(std::make_unique<Impl>()) {}

void Log::log(LogVerbosity verbosity, const std::string& message) const
{    
   m_pImpl->log(verbosity, message);   
}

Log::~Log() = default;