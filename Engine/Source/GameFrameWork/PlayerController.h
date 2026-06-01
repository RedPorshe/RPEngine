#pragma once
#include "Controller.h"

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

private:
    void handleKeyPress(int key, int scancode, int action, int mods);
    void handleMouseMove(double x, double y);
    void handleMouseButton(int button, int action, int mods, double x, double y);
    void handleMouseScroll(double xoffset, double yoffset);
    std::string controllerName;
};
}  // namespace RPE