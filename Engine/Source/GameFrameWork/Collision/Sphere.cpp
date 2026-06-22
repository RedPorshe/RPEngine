#include "Sphere.h"
using namespace RPE;

SphereComponent::SphereComponent(const std::string& inName, CObject* inOwner)  //
    : Super(inName, inOwner)
{
    setShape(ECollisionShape::Sphere);
}

void SphereComponent::tick(float deltaTime)
{
    Super::tick(deltaTime);
}

void SphereComponent::onDestroy()
{
    Super::onDestroy();
}

void SphereComponent::onBeginPlay()
{
    Super::onBeginPlay();
}

void SphereComponent::OnCreate()
{
    Super::OnCreate();
}

void SphereComponent::onBeginOverlap(CollisionComponent* other)
{
    Super::onBeginOverlap(other);
}

void SphereComponent::onEndOverlap(CollisionComponent* other)
{
    Super::onEndOverlap(other);
}
