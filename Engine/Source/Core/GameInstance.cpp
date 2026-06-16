#include "GameInstance.h"
#include "../GameFramework/GameObjects/WorldObject/WorldActor.h"
#include "../GameFramework/GameObjects/WorldObject/WorldPawn.h"
#include "../GameFramework/World/World.h"
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
    static int cccc2 = 0;
    if (cccc2 < 1)
    {
        RP_LOG(GameInstanceLog, Display, "Tick for {}", GetName());
        cccc2++;
    }
    world->Tick(deltaTime);
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
