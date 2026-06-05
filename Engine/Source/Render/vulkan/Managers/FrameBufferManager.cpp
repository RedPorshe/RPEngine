#include "FrameBufferManager.h"
#include "../vkContext.h"
#include "RenderpassManager.h"
#include "SwapchainManager.h"
#include "DeviceManager.h"
#include "Event/Event.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(FrameBufferLog);

using namespace RPE;

FrameBufferManager::FrameBufferManager()
{
    RP_LOG(FrameBufferLog, Display, "{} created", getName());
}

FrameBufferManager::~FrameBufferManager()
{
    shutdown();
}

bool FrameBufferManager::preInit(const WindowSettings& settings, const std::string& EngineName)
{
    return true;
}

bool FrameBufferManager::init()
{
    if (!m_contextPtr)
    {
        RP_LOG(FrameBufferLog, Error, "{} has no context ptr", getName());
        return false;
    }

    auto* swapchainMgr = m_contextPtr->getSwapchainManager();
    if (!swapchainMgr)
    {
        RP_LOG(FrameBufferLog, Error, "SwapchainManager is null");
        return false;
    }

    auto* renderPassMgr = m_contextPtr->getRenderpassManager();
    if (!renderPassMgr)
    {
        RP_LOG(FrameBufferLog, Error, "RenderPassManager is null");
        return false;
    }

    if (!createBuffers())
    {
        RP_LOG(FrameBufferLog, Error, "Failed to create framebuffers");
        return false;
    }

    m_isInitialized = true;
    RP_LOG(FrameBufferLog, Display, "{} initialized successfully with {} framebuffers", getName(), m_framebuffers.size());
    return true;
}

void FrameBufferManager::shutdown()
{
    if (isInitialized())
    {
        clearBuffers();
        m_isInitialized = false;
    }
    RP_LOG(FrameBufferLog, Display, "{} shutdown complete", getName());
}

std::string FrameBufferManager::getName()
{
    return "[Frame buffers Manager]";
}

void FrameBufferManager::onResize(int width, int height)
{
    if (!recreateBuffer())
    {
        RP_LOG(FrameBufferLog, Error, "Failed recreate frame buffers");
    }
}

const std::vector<VkFramebuffer>& RPE::FrameBufferManager::getFramebuffers() const
{
    return m_framebuffers;
}

void FrameBufferManager::clearBuffers()
{
    if (!m_contextPtr) return;

    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr) return;

    VkDevice device = deviceMgr->getDevice();
    if (device == VK_NULL_HANDLE) return;

    for (auto& framebuffer : m_framebuffers)
    {
        if (framebuffer != VK_NULL_HANDLE)
        {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
    }
    m_framebuffers.clear();
    RP_LOG(FrameBufferLog, Display, "Framebuffers cleared");
}

bool FrameBufferManager::createBuffers()
{
    if (!m_contextPtr)
    {
        RP_LOG(FrameBufferLog, Error, "{} has no context ptr", getName());
        return false;
    }

    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr)
    {
        RP_LOG(FrameBufferLog, Error, "DeviceManager is null");
        return false;
    }

    auto* swapchainMgr = m_contextPtr->getSwapchainManager();
    if (!swapchainMgr)
    {
        RP_LOG(FrameBufferLog, Error, "SwapchainManager is null");
        return false;
    }

    auto* renderPassMgr = m_contextPtr->getRenderpassManager();
    if (!renderPassMgr)
    {
        RP_LOG(FrameBufferLog, Error, "RenderPassManager is null");
        return false;
    }

    VkDevice device = deviceMgr->getDevice();
    VkRenderPass renderPass = renderPassMgr->getRenderPass();
    const auto& imageViews = swapchainMgr->getImageViews();
    VkExtent2D extent = swapchainMgr->getExtent();

    if (device == VK_NULL_HANDLE || renderPass == VK_NULL_HANDLE || imageViews.empty())
    {
        RP_LOG(FrameBufferLog, Error, "Invalid Vulkan components for framebuffer creation");
        return false;
    }

    m_framebuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); i++)
    {
        VkImageView attachments[] = {imageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_framebuffers[i]);
        if (result != VK_SUCCESS)
        {
            RP_LOG(FrameBufferLog, Error, "Failed to create framebuffer {}: {}", i, static_cast<int>(result));
            return false;
        }
    }

    RP_LOG(FrameBufferLog, Display, "Created {} framebuffers", m_framebuffers.size());
    return true;
}

bool FrameBufferManager::recreateBuffer()
{
    RP_LOG(FrameBufferLog, Display, "Recreating framebuffers...");

    if (!m_contextPtr) return false;

    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr) return false;

    VkDevice device = deviceMgr->getDevice();
    if (device == VK_NULL_HANDLE) return false;

    // Ждём завершения операций
    vkDeviceWaitIdle(device);

    // Очищаем старые
    clearBuffers();

    // Создаём новые
    if (!createBuffers())
    {
        RP_LOG(FrameBufferLog, Error, "Failed to recreate framebuffers");
        return false;
    }

    RP_LOG(FrameBufferLog, Display, "Framebuffers recreated successfully");
    return true;
}
