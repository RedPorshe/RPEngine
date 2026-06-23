#pragma once
#include "../GameObjects/Components/TransformComponent.h"
#include "../../Physics/PhysicsUtils.h"
#include "CollisionComponent.h"

namespace RPE
{
class CapsuleComponent : public CollisionComponent
{
    CHUDDO_DECLARE_CLASS(CapsuleComponent, CollisionComponent);

public:
    CapsuleComponent(const std::string& inName, CObject* inOwner, bool inTests = false);
    ~CapsuleComponent() = default;
    void tick(float deltaTime) override;
    void onDestroy() override;
    void onBeginPlay() override;
    void OnCreate() override;

protected:
    friend class PhysicsManager;
    void onBeginOverlap(CollisionComponent* other) override;
    void onEndOverlap(CollisionComponent* other) override;
};

REGISTER_CLASS_FACTORY(CapsuleComponent);
}  // namespace RPE
