#pragma once
#include "Core/Utility.h"
#include "Window/IWindow.h"

#include <string>

namespace RPE
{

class RHI : public NonCopyable
{
public:
    virtual ~RHI() = default;
    virtual bool preInit(WindowSettings, const std::string& EngineName) = 0;
    virtual bool init(IWindow* windowHandle) = 0;
    virtual void update() = 0;
    virtual void setEnginePtr(class Engine* ptr) = 0;
    virtual bool render() = 0;
    virtual void shutdown() = 0;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;

    virtual void onResize(int width, int height) = 0;
};
}  // namespace RPE