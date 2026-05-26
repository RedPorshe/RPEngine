#include "Log.h"
#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace RPE;

Log::Log() {}

void Log::log(LogVerbosity verbosity, const std::string& message) const 
	{
    const auto consolesink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    const auto m_consoleLogger = std::make_unique<spdlog::logger>("console", consolesink);
    m_consoleLogger->log(spdlog::level::info, message.c_str());
	}


Log::~Log() {}
