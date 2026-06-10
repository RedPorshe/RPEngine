#pragma once
#include "IVkManager.h"
#include "Window/IWindow.h"
#include <string>
#include <vulkan/vulkan.h>

namespace RPE
{
class CommandManager : public IVkManager
{
public:
    CommandManager();
    ~CommandManager() override;

    bool preInit(const WindowSettings& settings, const std::string& EngineName) override;
    bool init() override;
    void shutdown() override;
    std::string getName() override;
    VkCommandBuffer getCommandBuffer(uint32_t index) const;

private:
    bool createCommandPool();
    bool createCommandBuffers();

    VkCommandPool m_commandPool{VK_NULL_HANDLE};
    std::vector<VkCommandBuffer> m_commandBuffers;
};
}  // namespace RPE