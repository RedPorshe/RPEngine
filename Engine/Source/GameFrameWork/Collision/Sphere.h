#pragma once
#include "../GameObjects/Components/TransformComponent.h"
#include "../../Physics/PhysicsUtils.h"
#include "CollisionComponent.h"

namespace RPE
{
class SphereComponent : public CollisionComponent
{
    CHUDDO_DECLARE_CLASS(SphereComponent, CollisionComponent);

public:
    SphereComponent(const std::string& inName, CObject* inOwner);
    ~SphereComponent() = default;
    void tick(float deltaTime) override;
    void onDestroy() override;
    void onBeginPlay() override;
    void OnCreate() override;

protected:
    friend class PhysicsManager;
    void onBeginOverlap(CollisionComponent* other) override;
    void onEndOverlap(CollisionComponent* other) override;
};

REGISTER_CLASS_FACTORY(SphereComponent);
}  // namespace RPE
