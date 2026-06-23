#include "GameInstance.h"
#include "../GameFrameWork/GameObjects/WorldObject/WorldActor.h"
#include "../GameFrameWork/GameObjects/WorldObject/WorldPawn.h"
#include "../GameFrameWork/World/World.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(GameInstanceLog);

using namespace RPE;

RGameInstance::RGameInstance(const std::string& inDisplayName, CObject* inOwner)  //
    : Super(inDisplayName, inOwner)
{
    world = AddSubObject<WWorld>("main world");
}

RGameInstance::~RGameInstance()
{
    onDestroy();
}

void RGameInstance::Init()
{
    if (world)
        if (!world->hasLevel("main_level"))
        {
            world->createLevel("main_level");
        }
    bIsinitialized = true;
}

void RGameInstance::tick(float deltaTime)
{
    if (world)
    {
        world->Tick(deltaTime);
    }
}

void RGameInstance::onDestroy()
{
    world->clearLevels();
    bIsinitialized = false;
}

bool RGameInstance::isInitialized() const
{
    return bIsinitialized;
}

void RGameInstance::StartGame()
{
    if (world)
    {
        world->BeginPlay();
    }
}

WWorld* RGameInstance::GetWorld()
{
    return world ? world : nullptr;
}
