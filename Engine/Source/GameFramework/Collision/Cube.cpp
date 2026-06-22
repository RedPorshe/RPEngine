#include "Cube.h"
using namespace RPE;

CubeComponent::CubeComponent(const std::string& inName, CObject* inOwner)  //
    : Super(inName, inOwner)
{
    setShape(ECollisionShape::Cube);
}

void CubeComponent::tick(float deltaTime)
{
    Super::tick(deltaTime);
}

void CubeComponent::onDestroy()
{
    Super::onDestroy();
}

void CubeComponent::onBeginPlay()
{
    Super::onBeginPlay();
}

void CubeComponent::OnCreate()
{
    Super::OnCreate();
}

void CubeComponent::onBeginOverlap(CollisionComponent* other)
{
    Super::onBeginOverlap(other);
}

void CubeComponent::onEndOverlap(CollisionComponent* other)
{
    Super::onEndOverlap(other);
}
