#pragma once
#include "GameFrameWork/GameObjects/Components/TransformComponent.h"
#include "Physics/PhysicsUtils.h"

namespace RPE
{
class PhysicsManager;

class CollisionComponent : public WTransformComponent
{
    CHUDDO_DECLARE_CLASS(CollisionComponent, WTransformComponent);

public:
    CollisionComponent(const std::string& inName, CObject* inOwner, bool inTests = false);
    ~CollisionComponent() = default;
    void tick(float deltaTime) override;
    void onDestroy() override;
    void onBeginPlay() override;
    void OnCreate() override;
    std::vector<CollisionComponent*> getOverlappedComponents() const;
    float getRadius() const;
    FVector getCubeHalfSize() const { return m_CubeHalfSize; }
    float getHalfHeight() const { return m_halfHeight; }
    ECollisionShape getCollisionShape() const;
    void setRadius(float radius) { m_radius = radius; }
    void setCubeHalfSize(const FVector& halfSize);
    void setCubeHalfSize(float Size);
    void SetCubeSize(const FVector& Size);
    void SetCubeSize(float Size);

    void setHalfHeight(float halfHeight) { m_halfHeight = halfHeight; }
    // Plane methods
    FVector getPlaneNormal() const { return m_planeNormal; }
    FVector getPlanePoint() const { return m_planePoint; }
    FVector2D getPlaneSize() const { return m_planeSize; }
    void setPlane(const FVector& normal, const FVector& point, const FVector2D& size = FVector2D(10.0f, 10.0f));
    bool isInfinitePlane() const { return m_bIsInfinitePlane; }
    void setInfinitePlane(bool infinite) { m_bIsInfinitePlane = infinite; }
    void SetInTests(bool val) { bisIntest = val; }

protected:
    friend class PhysicsManager;
    virtual void onBeginOverlap(CollisionComponent* other);
    virtual void onEndOverlap(CollisionComponent* other);
    std::vector<CollisionComponent*> overlapedCollisions;
    float m_radius = 1.0f;
    FVector m_CubeHalfSize = FVector::One();
    float m_halfHeight = 9.0f;
    FVector m_planeNormal = FVector(0, 1, 0);
    FVector m_planePoint = FVector(0, 0, 0);
    FVector2D m_planeSize = FVector2D(10.0f, 10.0f);
    bool m_bIsInfinitePlane = false;
    ECollisionShape m_shape{ECollisionShape::NO_COLLISION};

public:
    void setShape(ECollisionShape shape);

    bool bisIntest{false};
};

REGISTER_CLASS_FACTORY(CollisionComponent);
}  // namespace RPE