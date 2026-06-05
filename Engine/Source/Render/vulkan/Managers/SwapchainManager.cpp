#include "SwapchainManager.h"
#include "../vkContext.h"
#include "DeviceManager.h"
#include "InstanceManager.h"
#include "Log/Log.h"

using namespace RPE;

DEFINE_LOG_CATEGORY_STATIC(LogSwapchainManager)

SwapchainManager::SwapchainManager()
{
    RP_LOG(LogSwapchainManager, Display, "{} class created", getName());
}

SwapchainManager::~SwapchainManager()
{
    shutdown();
}

bool SwapchainManager::preInit(const WindowSettings& settings, const std::string& EngineName)
{
    m_settings = settings;
    return true;
}

bool SwapchainManager::init()
{
    if (!m_contextPtr)
    {
        RP_LOG(LogSwapchainManager, Error, "VulkanContext is null");
        return false;
    }

    auto* deviceMgr = m_contextPtr->getDeviceManager();
    auto* instanceMgr = m_contextPtr->getInstanceManager();

    if (!deviceMgr || !instanceMgr)
    {
        RP_LOG(LogSwapchainManager, Error, "Required managers are null");
        return false;
    }

    VkDevice device = deviceMgr->getDevice();
    VkPhysicalDevice physicalDevice = deviceMgr->getPhysicalDevice();
    VkSurfaceKHR surface = instanceMgr->getSurface();

    if (device == VK_NULL_HANDLE || physicalDevice == VK_NULL_HANDLE || surface == VK_NULL_HANDLE)
    {
        RP_LOG(LogSwapchainManager, Error, "Vulkan components are not initialized");
        return false;
    }

    chooseSurfaceFormat();
    choosePresentMode();
    chooseExtent(m_settings);

    if (!createSwapchain())
    {
        RP_LOG(LogSwapchainManager, Error, "Failed to create swap chain");
        return false;
    }

    if (!createImageViews())
    {
        RP_LOG(LogSwapchainManager, Error, "Failed to create image views");
        return false;
    }

    m_isInitialized = true;
    RP_LOG(LogSwapchainManager, Display, "Swapchain initialized successfully");
    RP_LOG(LogSwapchainManager, Display, "  Format: {}, Extent: {}x{}", static_cast<int>(m_imageFormat), m_extent.width, m_extent.height);

    return true;
}

void SwapchainManager::shutdown()
{
    if (isInitialized())
    {
        cleanup();
        m_isInitialized = false;
    }
}

std::string SwapchainManager::getName()
{
    return "[Swapchain Manager]";
}

void SwapchainManager::onResize(int width, int height)
{
    if (!m_isInitialized) return;

    m_settings.width = width;
    m_settings.height = height;

    // Пересоздаём swap chain
    RecreateSwapchain();
}

VkSwapchainKHR RPE::SwapchainManager::getSwapchain() const
{
    return m_swapchain;
}

VkFormat RPE::SwapchainManager::getImageFormat() const
{
    return m_imageFormat;
}

VkExtent2D RPE::SwapchainManager::getExtent() const
{
    return m_extent;
}

const std::vector<VkImageView>& RPE::SwapchainManager::getImageViews() const
{
    return m_swapchainImageViews;
}

bool RPE::SwapchainManager::RecreateSwapchain()
{
    if (!m_contextPtr) return false;

    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr) return false;

    VkDevice device = deviceMgr->getDevice();
    if (device == VK_NULL_HANDLE) return false;

    // Ждём завершения работы устройства
    vkDeviceWaitIdle(device);

    // Очищаем старые ресурсы
    cleanup();

    // Пересоздаём
    chooseExtent(m_settings);

    if (!createSwapchain())
    {
        RP_LOG(LogSwapchainManager, Error, "Failed to recreate swap chain");
        return false;
    }

    if (!createImageViews())
    {
        RP_LOG(LogSwapchainManager, Error, "Failed to recreate image views");
        return false;
    }

    RP_LOG(LogSwapchainManager, Display, "Swap chain recreated successfully");

    return true;
}

bool SwapchainManager::createSwapchain()
{
    auto* deviceMgr = m_contextPtr->getDeviceManager();
    auto* instanceMgr = m_contextPtr->getInstanceManager();

    VkDevice device = deviceMgr->getDevice();
    VkPhysicalDevice physicalDevice = deviceMgr->getPhysicalDevice();
    VkSurfaceKHR surface = instanceMgr->getSurface();

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = m_imageFormat;
    createInfo.imageColorSpace = m_colorSpace;
    createInfo.imageExtent = m_extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto* deviceMgrPtr = deviceMgr;
    uint32_t queueFamilyIndices[] = {
        static_cast<uint32_t>(deviceMgrPtr->getGraphicsQueueIndex()), static_cast<uint32_t>(deviceMgrPtr->getPresentQueueIndex())};

    if (deviceMgrPtr->getGraphicsQueueIndex() != deviceMgrPtr->getPresentQueueIndex())
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = m_presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_swapchain);
    if (result != VK_SUCCESS)
    {
        RP_LOG(LogSwapchainManager, Error, "Failed to create swap chain: {}", static_cast<int>(result));
        return false;
    }

    // Получаем изображения swap chain
    vkGetSwapchainImagesKHR(device, m_swapchain, &imageCount, nullptr);
    m_swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, m_swapchain, &imageCount, m_swapchainImages.data());

    RP_LOG(LogSwapchainManager, Display, "Swap chain created with {} images", imageCount);

    return true;
}

bool SwapchainManager::createImageViews()
{
    auto* deviceMgr = m_contextPtr->getDeviceManager();
    VkDevice device = deviceMgr->getDevice();

    m_swapchainImageViews.resize(m_swapchainImages.size());

    for (size_t i = 0; i < m_swapchainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_swapchainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_imageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VkResult result = vkCreateImageView(device, &createInfo, nullptr, &m_swapchainImageViews[i]);
        if (result != VK_SUCCESS)
        {
            RP_LOG(LogSwapchainManager, Error, "Failed to create image view {}", i);
            return false;
        }
    }

    return true;
}

void SwapchainManager::chooseSurfaceFormat()
{
    auto* deviceMgr = m_contextPtr->getDeviceManager();
    auto* instanceMgr = m_contextPtr->getInstanceManager();

    VkPhysicalDevice physicalDevice = deviceMgr->getPhysicalDevice();
    VkSurfaceKHR surface = instanceMgr->getSurface();

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());

    for (const auto& format : formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            m_imageFormat = format.format;
            m_colorSpace = format.colorSpace;
            return;
        }
    }
    m_imageFormat = formats[0].format;
    m_colorSpace = formats[0].colorSpace;
}

void SwapchainManager::choosePresentMode()
{
    auto* deviceMgr = m_contextPtr->getDeviceManager();
    auto* instanceMgr = m_contextPtr->getInstanceManager();

    VkPhysicalDevice physicalDevice = deviceMgr->getPhysicalDevice();
    VkSurfaceKHR surface = instanceMgr->getSurface();

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

    for (const auto& mode : presentModes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            m_presentMode = mode;
            return;
        }
    }

    m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
}

void SwapchainManager::chooseExtent(const WindowSettings& settings)
{
    auto* deviceMgr = m_contextPtr->getDeviceManager();
    auto* instanceMgr = m_contextPtr->getInstanceManager();

    VkPhysicalDevice physicalDevice = deviceMgr->getPhysicalDevice();
    VkSurfaceKHR surface = instanceMgr->getSurface();

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        m_extent = capabilities.currentExtent;
    }
    else
    {
        m_extent.width =
            std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, static_cast<uint32_t>(settings.width)));
        m_extent.height = std::max(
            capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, static_cast<uint32_t>(settings.height)));
    }
}

void SwapchainManager::cleanup()
{
    auto* deviceMgr = m_contextPtr ? m_contextPtr->getDeviceManager() : nullptr;
    VkDevice device = deviceMgr ? deviceMgr->getDevice() : VK_NULL_HANDLE;

    if (device != VK_NULL_HANDLE)
    {
        for (auto& imageView : m_swapchainImageViews)
        {
            if (imageView != VK_NULL_HANDLE)
            {
                vkDestroyImageView(device, imageView, nullptr);
            }
        }

        if (m_swapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(device, m_swapchain, nullptr);
            m_swapchain = VK_NULL_HANDLE;
        }
    }

    m_swapchainImages.clear();
    m_swapchainImageViews.clear();
}
