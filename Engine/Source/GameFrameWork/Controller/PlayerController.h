#pragma once
#include "Controller.h"
#include "Input/KeyCodes.h"
#include <functional>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "GameFrameWork/GameObjects/WorldObject/WorldActor.h"

namespace RPE
{

class PlayerController : public WActor, public IController
{
    CHUDDO_DECLARE_CLASS(PlayerController, WActor);

public:
    PlayerController(const std::string& inDisplayName = "PlayerController", CObject* inOwner = nullptr);
    ~PlayerController() override;
    void BeginPlay() override;
    void onKeyPress(int key, int scancode, int action, int mods) override;
    void onMouseMove(double x, double y) override;
    void onMouseButton(int button, int action, int mods, double x, double y) override;
    void onMouseScroll(double xoffset, double yoffset) override;
    std::string name() const override;
    void setName(const std::string& newName) override;

    class WInputComponent* getPlayerInputComponent() const;
    class WPawn* getControlledObject() const;
    void setControlledObject(class WPawn* object);

private:
    void update(float deltaTime);
    void handleKeyPress(int key, int scancode, int action, int mods);
    void handleMouseMove(double x, double y);
    void handleMouseButton(int button, int action, int mods, double x, double y);
    void handleMouseScroll(double xoffset, double yoffset);

    std::string controllerName;
    class WPawn* m_ControlledObject = nullptr;
};

REGISTER_CLASS_FACTORY(PlayerController);

}  // namespace RPE