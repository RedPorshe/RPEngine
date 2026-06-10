#pragma once
#include "Core/SystemObject.h"

namespace RPE
{

class WorldObject : public CObject
{
    CHUDDO_DECLARE_CLASS(WorldObject, CObject);

public:
    WorldObject(const std::string& inDisplayName = "WorldObject", CObject* inOwner = nullptr);
    virtual ~WorldObject();

    class InputComponent* getInputComponent();
    void update(float deltaTime);

protected:
    void setupInputBindings();
    void moveRight(float value);
    void moveForward(float value);
    void look(float value);
    void lookUp(float value);
    void jump();

    class InputComponent* m_inputComponent = nullptr;
    float m_positionX = 0.0f;
    float m_positionY = 0.0f;
    float m_rotation = 0.0f;
};

}  // namespace RPE
namespace RPE
{
REGISTER_CLASS_FACTORY(WorldObject);
}