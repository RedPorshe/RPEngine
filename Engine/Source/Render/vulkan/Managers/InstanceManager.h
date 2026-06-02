#pragma once
#include "IVkManager.h"
#include "Window/IWindow.h"
#include <string>
#include <vulkan/vulkan.h>

namespace RPE
{
class InstanceManager : public IVkManager
{
public:
    InstanceManager();
    ~InstanceManager() override;

    bool preInit(const WindowSettings& settings, const std::string& EngineName) override;
    bool init() override;
    void shutdown() override;
    std::string getName() override;
    VkInstance getInstance() const;

private:
    bool createInstance(const WindowSettings& settings, const std::string& EngineName);
    VkInstance instance{VK_NULL_HANDLE};
};
}  // namespace RPE