#pragma once
#include "WorldActorComponent.h"
#include <vector>
#include "Math/MathTypes.h"

namespace RPE
{

class WActor;

class WTransformComponent : public WActorComponent
{
    CHUDDO_DECLARE_CLASS(WTransformComponent, WActorComponent);

public:
    WTransformComponent(const std::string& inDisplayName = "Actor Component", CObject* inOwner = nullptr);
    virtual ~WTransformComponent();

    void tick(float deltaTime) override;
    void onDestroy() override;
    void onBeginPlay() override;

protected:
    FVector m_location = FVector::Zero();
    FQuat m_rotation = FQuat::Identity();
    FVector m_scale = FVector::One();
    FMat4 m_transformMatrix = FMat4(1.0f);
};

}  // namespace RPE
namespace RPE
{
REGISTER_CLASS_FACTORY(WTransformComponent);
}