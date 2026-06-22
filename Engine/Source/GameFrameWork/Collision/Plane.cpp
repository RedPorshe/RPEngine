#include "Plane.h"

using namespace RPE;

PlaneComponent::PlaneComponent(const std::string& inName, CObject* inOwner)  //
    : Super(inName, inOwner)
{
    setShape(ECollisionShape::Plane);
}

void PlaneComponent::tick(float deltaTime)
{
    Super::tick(deltaTime);
}

void PlaneComponent::onDestroy()
{
    Super::onDestroy();
}

void PlaneComponent::onBeginPlay()
{
    Super::onBeginPlay();
}

void PlaneComponent::OnCreate()
{
    Super::OnCreate();
}

void PlaneComponent::onBeginOverlap(CollisionComponent* other)
{
    Super::onBeginOverlap(other);
}

void PlaneComponent::onEndOverlap(CollisionComponent* other)
{
    Super::onEndOverlap(other);
}
