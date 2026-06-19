#pragma once
#include "Core/SystemObject.h"
#include "WorldActor.h"
namespace RPE
{
class IController;

class WPawn : public WActor  // aka APawn
{
    CHUDDO_DECLARE_CLASS(WPawn, WActor);

public:
    WPawn(const std::string& inDisplayName = "WorldPawnObject", CObject* inOwner = nullptr);
    virtual ~WPawn();

    class WInputComponent* getInputComponent();

    void Tick(float DeltaTime) override;
    void EndPlay() override;

    void BeginPlay() override;
    virtual void onPossess(IController* controller);
    class PlayerController* getPlayerController() const;
    IController* getController() const;
    void unPosses();

protected:
    void setupInputBindings(class WInputComponent* inputComponent);

    IController* m_controller = nullptr;
    class WInputComponent* m_inputComponent = nullptr;
    float m_positionX = 0.0f;
    float m_positionY = 0.0f;
    float m_rotation = 0.0f;
};

}  // namespace RPE
namespace RPE
{
REGISTER_CLASS_FACTORY(WPawn);
}