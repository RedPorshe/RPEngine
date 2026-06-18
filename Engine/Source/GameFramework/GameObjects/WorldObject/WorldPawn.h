#pragma once
#include "Core/SystemObject.h"
#include "WorldActor.h"
namespace RPE
{

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

protected:
    void setupInputBindings();
    void moveRight(float value);
    void moveForward(float value);
    void look(float value);
    void lookUp(float value);
    void jump();

    class WInputComponent* m_inputComponent = nullptr;
    float m_positionX = 0.0f;
    float m_positionY = 0.0f;
    float m_rotation = 0.0f;
    void Quit();
};

}  // namespace RPE
namespace RPE
{
REGISTER_CLASS_FACTORY(WPawn);
}