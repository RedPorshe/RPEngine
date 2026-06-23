#include "SyncManager.h"
#include "Render/vulkan/vkContext.h"
#include "SwapchainManager.h"
#include "DeviceManager.h"
#include "Log/Log.h"

using namespace RPE;

DEFINE_LOG_CATEGORY_STATIC(SyncManagerLog);

SyncManager::SyncManager() {}

SyncManager::~SyncManager() {}

bool SyncManager::preInit(const WindowSettings& settings, const std::string& EngineName)
{
    return true;
}

bool SyncManager::init()
{
    if (!m_contextPtr)
    {
        RP_LOG(SyncManagerLog, Error, "Context ptr is null");
        return false;
    }
    m_MAX_FRAMES_IN_FLIGHT = m_contextPtr->getMaxFrames();
    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr)
    {
        RP_LOG(SyncManagerLog, Error, "Device manager is null");
        return false;
    }

    VkDevice device = deviceMgr->getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(SyncManagerLog, Error, "Logical device is null");
        return false;
    }

    if (!createSyncs())
    {
        RP_LOG(SyncManagerLog, Error, "Failed to create synchronization objects");
        return false;
    }

    m_isInitialized = true;
    RP_LOG(SyncManagerLog, Display, "SyncManager initialized successfully");
    return true;
}

void SyncManager::shutdown()
{
    if (!m_isInitialized) return;

    destroySyncs();
    m_isInitialized = false;
    RP_LOG(SyncManagerLog, Display, "SyncManager shutdown complete");
}

std::string SyncManager::getName()
{
    return "[Sync Manager]";
}

VkSemaphore SyncManager::getImageAvailableSemaphore(uint32_t index) const
{
    if (index >= m_imageAvailableSemaphores.size())
    {
        RP_LOG(SyncManagerLog, Error, "Image available semaphore index {} out of range", index);
        return VK_NULL_HANDLE;
    }
    return m_imageAvailableSemaphores[index];
}

VkSemaphore SyncManager::getRenderFinishedSemaphore(uint32_t index) const
{
    if (index >= m_renderFinishedSemaphores.size())
    {
        RP_LOG(SyncManagerLog, Error, "Render finished semaphore index {} out of range", index);
        return VK_NULL_HANDLE;
    }
    return m_renderFinishedSemaphores[index];
}

VkFence SyncManager::getInFlightFence(uint32_t index) const
{
    if (index >= m_inFlightFences.size())
    {
        RP_LOG(SyncManagerLog, Error, "In-flight fence index {} out of range", index);
        return VK_NULL_HANDLE;
    }
    return m_inFlightFences[index];
}

bool SyncManager::createSyncs()
{
    auto* deviceMgr = m_contextPtr->getDeviceManager();
    VkDevice device = deviceMgr->getDevice();

    m_imageAvailableSemaphores.resize(m_MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(m_MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(m_MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;  // Создаём в сигнальном состоянии

    for (uint32_t i = 0; i < m_MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkResult result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]);
        if (result != VK_SUCCESS)
        {
            RP_LOG(SyncManagerLog, Error, "Failed to create image available semaphore {}: {}", i, static_cast<int>(result));
            return false;
        }

        result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]);
        if (result != VK_SUCCESS)
        {
            RP_LOG(SyncManagerLog, Error, "Failed to create render finished semaphore {}: {}", i, static_cast<int>(result));
            return false;
        }

        result = vkCreateFence(device, &fenceInfo, nullptr, &m_inFlightFences[i]);
        if (result != VK_SUCCESS)
        {
            RP_LOG(SyncManagerLog, Error, "Failed to create in-flight fence {}: {}", i, static_cast<int>(result));
            return false;
        }
    }

    RP_LOG(SyncManagerLog, Display, "Created {} synchronization objects", m_MAX_FRAMES_IN_FLIGHT);
    return true;
}

void SyncManager::destroySyncs()
{
    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr) return;

    VkDevice device = deviceMgr->getDevice();
    if (device == VK_NULL_HANDLE) return;

    for (uint32_t i = 0; i < m_MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (m_imageAvailableSemaphores[i] != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(device, m_imageAvailableSemaphores[i], nullptr);
        }
        if (m_renderFinishedSemaphores[i] != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(device, m_renderFinishedSemaphores[i], nullptr);
        }
        if (m_inFlightFences[i] != VK_NULL_HANDLE)
        {
            vkDestroyFence(device, m_inFlightFences[i], nullptr);
        }
    }

    m_imageAvailableSemaphores.clear();
    m_renderFinishedSemaphores.clear();
    m_inFlightFences.clear();

    RP_LOG(SyncManagerLog, Display, "Destroyed all synchronization objects");
}