#include "TransformComponent.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(TranformCLog);
using namespace RPE;

WTransformComponent::WTransformComponent(const std::string& inDisplayName, CObject* inOwner)  //
    : Super(inDisplayName, inOwner)
{
}

WTransformComponent::~WTransformComponent() {}

void WTransformComponent::tick(float deltaTime)
{
    Super::tick(deltaTime);
}

void WTransformComponent::onDestroy()
{
    Super::onDestroy();
}

void WTransformComponent::onBeginPlay()
{
    Super::onBeginPlay();
}
