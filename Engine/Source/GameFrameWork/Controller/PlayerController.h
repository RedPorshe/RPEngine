#pragma once
#include "Controller.h"
#include "Input/KeyCodes.h"
#include <functional>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

namespace RPE
{

class PlayerController : public IController
{
public:
    PlayerController();
    ~PlayerController() override;

    void onKeyPress(int key, int scancode, int action, int mods) override;
    void onMouseMove(double x, double y) override;
    void onMouseButton(int button, int action, int mods, double x, double y) override;
    void onMouseScroll(double xoffset, double yoffset) override;
    std::string name() const override;
    void setName(const std::string& newName) override;

    class InputComponent* getPlayerInputComponent() const;
    class WorldObject* getControlledObject() const;
    void setControlledObject(class WorldObject* object);

private:
    void update(float deltaTime);
    void handleKeyPress(int key, int scancode, int action, int mods);
    void handleMouseMove(double x, double y);
    void handleMouseButton(int button, int action, int mods, double x, double y);
    void handleMouseScroll(double xoffset, double yoffset);

    std::string controllerName;
    class WorldObject* m_ControlledObject = nullptr;
};

}  // namespace RPE