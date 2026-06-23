#include "CommandManager.h"
#include "Render/vulkan/vkContext.h"
#include "DeviceManager.h"
#include "SwapchainManager.h"
#include "RenderpassManager.h"
#include "FrameBufferManager.h"
#include "PipelineManager.h"
#include "Log/Log.h"

using namespace RPE;
DEFINE_LOG_CATEGORY_STATIC(CommandBufferLog);

CommandManager::CommandManager() {}

CommandManager::~CommandManager() {}

bool CommandManager::preInit(const WindowSettings& settings, const std::string& EngineName)
{
    return true;
}

bool CommandManager::init()
{
    if (!m_contextPtr)
    {
        RP_LOG(CommandBufferLog, Error, "Context ptr is null");
        return false;
    }

    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr)
    {
        RP_LOG(CommandBufferLog, Error, "Device manager is null");
        return false;
    }

    VkDevice device = deviceMgr->getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(CommandBufferLog, Error, "Logical device is null");
        return false;
    }

    if (!createCommandPool())
    {
        RP_LOG(CommandBufferLog, Error, "Failed to create command pool");
        return false;
    }

    if (!createCommandBuffers())
    {
        RP_LOG(CommandBufferLog, Error, "Failed to create command buffers");
        return false;
    }

    m_isInitialized = true;
    RP_LOG(CommandBufferLog, Display, "CommandManager initialized with {} command buffers", m_commandBuffers.size());
    return true;
}

void CommandManager::shutdown()
{
    if (!m_contextPtr) return;

    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr) return;

    VkDevice device = deviceMgr->getDevice();
    if (device == VK_NULL_HANDLE) return;

    // Command buffers автоматически уничтожаются при уничтожении command pool
    m_commandBuffers.clear();

    if (m_commandPool != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(device, m_commandPool, nullptr);
        m_commandPool = VK_NULL_HANDLE;
        RP_LOG(CommandBufferLog, Display, "Command pool destroyed");
    }

    m_isInitialized = false;
}

std::string CommandManager::getName()
{
    return "[Command buffer Manager]";
}

VkCommandBuffer CommandManager::getCommandBuffer(uint32_t index) const
{
    if (index >= m_commandBuffers.size())
    {
        RP_LOG(CommandBufferLog, Error, "Command buffer index {} out of range (size: {})", index, m_commandBuffers.size());
        return VK_NULL_HANDLE;
    }
    return m_commandBuffers[index];
}

bool CommandManager::createCommandPool()
{
    auto* deviceMgr = m_contextPtr->getDeviceManager();
    VkDevice device = deviceMgr->getDevice();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = static_cast<uint32_t>(deviceMgr->getGraphicsQueueIndex());

    VkResult result = vkCreateCommandPool(device, &poolInfo, nullptr, &m_commandPool);
    if (result != VK_SUCCESS)
    {
        RP_LOG(CommandBufferLog, Error, "Failed to create command pool: {}", static_cast<int>(result));
        return false;
    }

    RP_LOG(CommandBufferLog, Display, "Command pool created successfully");
    return true;
}

bool CommandManager::createCommandBuffers()
{
    if (!m_contextPtr)
    {
        RP_LOG(CommandBufferLog, Error, "Context ptr is null");
        return false;
    }

    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr)
    {
        RP_LOG(CommandBufferLog, Error, "Device manager is null");
        return false;
    }

    VkDevice device = deviceMgr->getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(CommandBufferLog, Error, "Logical device is null");
        return false;
    }
    auto* swapchainMgr = m_contextPtr->getSwapchainManager();
    if (!swapchainMgr)
    {
        RP_LOG(CommandBufferLog, Error, "Swapchain manager is null");
        return false;
    }

    uint32_t imageCount = static_cast<uint32_t>(swapchainMgr->getImageViews().size());
    m_commandBuffers.resize(imageCount);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = imageCount;

    VkResult result = vkAllocateCommandBuffers(device, &allocInfo, m_commandBuffers.data());
    if (result != VK_SUCCESS)
    {
        RP_LOG(CommandBufferLog, Error, "Failed to allocate command buffers: {}", static_cast<int>(result));
        return false;
    }

    RP_LOG(CommandBufferLog, Display, "Allocated {} command buffers", imageCount);
    return true;
}
