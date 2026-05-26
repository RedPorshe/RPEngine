#include "Engine.h"

#include "Log/Log.h"
#include <format>

using namespace RPE;
DEFINE_LOG_CATEGORY_STATIC(EngineLog);

Engine::Engine()
{
    RP_LOG(EngineLog, Display, "Initializing Engine, version: {}", version());
    RP_LOG(EngineLog, Warning, "Adsada");
    RP_LOG(EngineLog, Log, "Adsada");
    RP_LOG_DEBUG(EngineLog, Warning, "Adsada");
}
Engine::~Engine() {}

void Engine::Run() {}
