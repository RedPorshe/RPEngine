#pragma once
#include "IVkManager.h"
#include "Window/IWindow.h"
#include <string>
#include <vulkan/vulkan.h>

namespace RPE
{
class RenderPassManager : public IVkManager
{
public:
    RenderPassManager();
    ~RenderPassManager() override;

    bool preInit(const WindowSettings& settings, const std::string& EngineName) override;
    bool init() override;
    void shutdown() override;
    std::string getName() override;
    void onResize(int width, int height) override;
    VkRenderPass getRenderPass() const;

private:
    bool createRenderPass();
    bool RecreateRenderPass();

    VkRenderPass m_renderPass{VK_NULL_HANDLE};
    VkFormat m_swapchainFormat{VK_FORMAT_UNDEFINED};
};
}  // namespace RPE