#include "Capsule.h"

using namespace RPE;

CapsuleComponent::CapsuleComponent(const std::string& inName, CObject* inOwner)  //
    : Super(inName, inOwner)
{
    setShape(ECollisionShape::Capsule);
}

void CapsuleComponent::tick(float deltaTime)
{
    Super::tick(deltaTime);
}

void CapsuleComponent::onDestroy()
{
    Super::onDestroy();
}

void CapsuleComponent::onBeginPlay()
{
    Super::onBeginPlay();
}

void CapsuleComponent::OnCreate()
{
    Super::OnCreate();
}

void CapsuleComponent::onBeginOverlap(CollisionComponent* other)
{
    Super::onBeginOverlap(other);
}

void CapsuleComponent::onEndOverlap(CollisionComponent* other)
{
    Super::onEndOverlap(other);
}
