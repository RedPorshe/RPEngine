#pragma once

namespace RPE
{
class WorldObject
{
public:
    WorldObject();
    ~WorldObject();

    class InputComponent* getInputComponent();
    void update(float deltaTime);

private:
    void setupInputBindings();
    void moveRight(float value);
    void moveForward(float value);
    void look(float value);
    void lookUp(float value);
    void jump();

    class InputComponent* m_inputComponent;

    // Игровое состояние
    float m_positionX = 0.0f;
    float m_positionY = 0.0f;
    float m_rotation = 0.0f;
};
}  // namespace RPE