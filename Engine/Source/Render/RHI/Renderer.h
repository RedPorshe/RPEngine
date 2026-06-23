#pragma once
#include "Core/Utility.h"
#include "Window/IWindow.h"
#include "Render/FrameInfo.h"

#include <string>

namespace RPE
{

class RHI : public NonCopyable
{
public:
    RHI() = default;
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
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

protected:
    int m_height = 800;
    int m_width = 600;
    FrameInfo m_FrameInfo;
};
}  // namespace RPE