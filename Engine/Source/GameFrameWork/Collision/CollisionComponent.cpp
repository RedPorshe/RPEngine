#include "CollisionComponent.h"
#include "../../Physics/PhysicsManager.h"
#include "Log/Log.h"
#include "Core/Engine.h"

#include <algorithm>

DEFINE_LOG_CATEGORY_STATIC(CollisionLog);

using namespace RPE;

CollisionComponent::CollisionComponent(const std::string& inName, CObject* inOwner)  //
    : Super(inName, inOwner)
{
    overlapedCollisions.clear();
    SetMovableState(EMovableState::Dynamic);
}

void CollisionComponent::tick(float deltaTime)
{
    Super::tick(deltaTime);
}

void CollisionComponent::onDestroy()
{
    Super::onDestroy();
    auto& engine = Engine::Get();
    if (&engine)
    {
        auto PhysicMgr = engine.getPhysicsManager();
        if (PhysicMgr)
        {
            PhysicMgr->unregisterCollisionComponent(this);
        }
    }
    overlapedCollisions.clear();
}

void CollisionComponent::onBeginPlay()
{
    Super::onBeginPlay();
    overlapedCollisions.clear();
}

void CollisionComponent::OnCreate()
{
    Super::OnCreate();
    RP_LOG(CollisionLog, Display, "[{}] OnCreate called", GetName());
    auto& engine = Engine::Get();
    if (&engine)
    {
        auto PhysicMgr = engine.getPhysicsManager();
        if (PhysicMgr)
        {
            PhysicMgr->registerCollisionComponent(this);
        }
    }
}

std::vector<CollisionComponent*> CollisionComponent::getOverlappedComponents() const
{
    return overlapedCollisions;
}

float CollisionComponent::getRadius() const
{
    return m_radius;
}

ECollisionShape CollisionComponent::getCollisionShape() const
{
    return m_shape;
}

void CollisionComponent::setCubeHalfSize(const FVector& halfSize)
{
    if (halfSize.IsZero()) return;
    m_CubeHalfSize = halfSize;
}

void CollisionComponent::setCubeHalfSize(float Size)
{
    if (CEMath::IsZero(Size)) return;
    setCubeHalfSize(FVector(Size, Size, Size));
}

void RPE::CollisionComponent::SetCubeSize(const FVector& Size)
{
    if (Size.IsZero()) return;
    if (Size.x < 0.f || Size.y < 0.f || Size.z < 0.f) return;
    FVector halfsize = Size / 2;
    setCubeHalfSize(halfsize);
}

void RPE::CollisionComponent::SetCubeSize(float Size)
{
    if (CEMath::IsZero(Size)) return;
    if (Size < 0.f) return;
    SetCubeSize(FVector(Size, Size, Size));
}

void CollisionComponent::setPlane(const FVector& normal, const FVector& point, const FVector2D& size)
{
    m_planeNormal = normal.Normalized();
    m_planePoint = point;
    m_planeSize = size;

    if (size.LengthSquared() < 0.0001f)
    {
        m_bIsInfinitePlane = true;
    }
    else
    {
        m_bIsInfinitePlane = false;
    }
}

void CollisionComponent::onBeginOverlap(CollisionComponent* other)
{
    if (!other) return;
    auto it = std::find(overlapedCollisions.begin(), overlapedCollisions.end(), other);
    if (it != overlapedCollisions.end()) return;
    overlapedCollisions.push_back(other);
    RP_LOG(CollisionLog, Display, "{} Start overlapping with {} component", GetName(), other->GetName());
    auto location = getLocation();
    float x = location.x;
    float y = location.y;
    float z = location.z;
    RP_LOG(CollisionLog, Display, "Collision {} start overlap position is {},{},{}", GetName(), x, y, z);
    location = getRelativeLocation();
    x = location.x;
    y = location.y;
    z = location.z;
    RP_LOG(CollisionLog, Display, "Collision {} start overlap  relative position is {},{},{}", GetName(), x, y, z);
}

void CollisionComponent::onEndOverlap(CollisionComponent* other)
{
    if (!other) return;
    auto it = std::find(overlapedCollisions.begin(), overlapedCollisions.end(), other);
    if (it == overlapedCollisions.end()) return;
    RP_LOG(CollisionLog, Display, "{} End overlapping with {} component", GetName(), other->GetName());
    auto location = getLocation();
    float x = location.x;
    float y = location.y;
    float z = location.z;
    RP_LOG(CollisionLog, Display, "Collision {} End overlap position is {},{},{}", GetName(), x, y, z);
    location = getRelativeLocation();
    x = location.x;
    y = location.y;
    z = location.z;
    RP_LOG(CollisionLog, Display, "Collision {} End overlap  relative position is {},{},{}", GetName(), x, y, z);
    overlapedCollisions.erase(it);
}

void CollisionComponent::setShape(ECollisionShape shape)
{
    m_shape = shape;
}
