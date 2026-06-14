#include "GameInstance.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(GameInstanceLog);

using namespace RPE;

RGameInstance::RGameInstance(const std::string& inDisplayName, CObject* inOwner)  //
    : Super(inDisplayName, inOwner)
{
}

RGameInstance::~RGameInstance()
{
    onDestroy();
}

void RGameInstance::Init() {}

void RGameInstance::tick(float deltaTime) {}

void RGameInstance::onDestroy() {}
