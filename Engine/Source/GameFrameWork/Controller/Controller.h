#pragma once
#include <string>
#include "Core/Utility.h"

namespace RPE
{
class IController : public NonCopyable
{

public:
    virtual ~IController() = default;
    virtual void onKeyPress(int key, int scancode, int action, int mods) = 0;
    virtual void onMouseMove(double x, double y) = 0;
    virtual void onMouseButton(int button, int action, int mods, double x, double y) = 0;
    virtual void onMouseScroll(double xoffset, double yoffset) = 0;
    virtual std::string name() const = 0;
    virtual void setName(const std::string& newName) = 0;
};

}  // namespace RPE