#pragma once
#include "Core/Utility.h"
#include "Window/IWindow.h"
#include <string>

namespace RPE
{
class IVkManager : public NonCopyable
{
public:
    virtual ~IVkManager() = default;
    virtual bool preInit(const WindowSettings& settings, const std::string& EngineName) = 0;
    virtual bool init() = 0;
    virtual void shutdown() = 0;
    virtual void onResize(int width, int height) {}
    virtual std::string getName() = 0;
    void setVkContextPtr(class VulkanContext* ctx) { m_contextPtr = ctx; }

private:
    class VulkanContext* m_contextPtr{nullptr};
};
}  // namespace RPE