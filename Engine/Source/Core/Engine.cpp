#include "Engine.h"

#include "Log/Log.h"
#include <format>

using namespace RPE;

Engine::Engine()
{
    Log::getInstance().log(LogVerbosity::Dysplay, 
                           std::format( "Initializing RedPorshe Engine, version: {}", version()));
    Log::getInstance().log(LogVerbosity::Warning, 
                           std::format( "Initializing RedPorshe Engine, version: {}", version()));
    Log::getInstance().log(LogVerbosity::Error, 
                           std::format( "Initializing RedPorshe Engine, version: {}", version()));
    Log::getInstance().log(LogVerbosity::Fatal, 
                           std::format( "Initializing RedPorshe Engine, version: {}", version()));

}

Engine::~Engine()
{
     Log::getInstance().log(LogVerbosity::Dysplay, "RedPorshe Engine destroyed!");
}

void Engine::Run() {}
