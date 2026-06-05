#pragma once
#include "IVkManager.h"
#include "Window/IWindow.h"
#include <string>
#include <vulkan/vulkan.h>

#include "Event/Event.h"
#include "Event/InputEvent.h"

namespace RPE
{
class SwapchainManager : public IVkManager
{
public:
    SwapchainManager();
    ~SwapchainManager() override;

    bool preInit(const WindowSettings& settings, const std::string& EngineName) override;
    bool init() override;
    void shutdown() override;
    std::string getName() override;
    void onResize(int width, int height) override;
    VkSwapchainKHR getSwapchain() const;
    VkFormat getImageFormat() const;
    VkExtent2D getExtent() const;
    const std::vector<VkImageView>& getImageViews() const;
    bool RecreateSwapchain();

private:
    bool createSwapchain();
    bool createImageViews();
    void chooseSurfaceFormat();
    void choosePresentMode();
    void chooseExtent(const WindowSettings& settings);
    void cleanup();

    VkSwapchainKHR m_swapchain{VK_NULL_HANDLE};
    VkFormat m_imageFormat{VK_FORMAT_UNDEFINED};
    VkColorSpaceKHR m_colorSpace{VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    VkPresentModeKHR m_presentMode{VK_PRESENT_MODE_FIFO_KHR};
    VkExtent2D m_extent{};
    std::vector<VkImage> m_swapchainImages;
    std::vector<VkImageView> m_swapchainImageViews;
    WindowSettings m_settings;
};
}  // namespace RPE