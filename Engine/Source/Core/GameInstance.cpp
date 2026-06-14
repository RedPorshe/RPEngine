#include "GameInstance.h"
#include "../GameFramework/GameObjects/WorldObject/WorldActor.h"
#include "../GameFramework/GameObjects/WorldObject/WorldPawn.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(GameInstanceLog);

using namespace RPE;

RGameInstance::RGameInstance(const std::string& inDisplayName, CObject* inOwner)  //
    : Super(inDisplayName, inOwner)
{
    world = AddSubObject<WActor>("world");
    level = world->AddSubObject<WActor>("level");
    auto pawn = level->AddSubObject<WPawn>("Pawn");
}

RGameInstance::~RGameInstance()
{
    onDestroy();
}

void RGameInstance::Init() {}

void RGameInstance::tick(float deltaTime)
{
    static int cccc2 = 0;
    if (cccc2 < 1)
    {
        RP_LOG(GameInstanceLog, Display, "Tick for {}", GetName());
        cccc2++;
    }
    world->Tick(deltaTime);
    level->Tick(deltaTime);
    if (const auto& pawn = dynamic_cast<WPawn*>(level->FindOwned("Pawn")))
    {
        pawn->Tick(deltaTime);
    }
}

void RGameInstance::onDestroy() {}
