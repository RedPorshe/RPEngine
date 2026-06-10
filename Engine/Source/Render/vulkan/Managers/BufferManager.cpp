#include "BufferManager.h"
#include "../vkContext.h"
#include "DeviceManager.h"
#include "Log/Log.h"
#include <cstring>

DEFINE_LOG_CATEGORY_STATIC(BufferManagerLog);

using namespace RPE;

BufferManager::BufferManager() {}

BufferManager::~BufferManager()
{
    shutdown();
}

bool BufferManager::preInit(const WindowSettings& settings, const std::string& EngineName)
{
    return true;
}

bool BufferManager::init()
{
    if (!m_contextPtr)
    {
        RP_LOG(BufferManagerLog, Error, "Context ptr is null");
        return false;
    }

    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr)
    {
        RP_LOG(BufferManagerLog, Error, "Device manager is null");
        return false;
    }

    m_device = deviceMgr->getDevice();
    m_physicalDevice = deviceMgr->getPhysicalDevice();

    if (m_device == VK_NULL_HANDLE || m_physicalDevice == VK_NULL_HANDLE)
    {
        RP_LOG(BufferManagerLog, Error, "Device or PhysicalDevice is null");
        return false;
    }

    if (!initTransferQueue())
    {
        RP_LOG(BufferManagerLog, Warning, "Failed to initialize transfer queue, buffer copying may not work");
    }

    m_isInitialized = true;
    RP_LOG(BufferManagerLog, Display, "BufferManager initialized successfully");
    return true;
}

void BufferManager::shutdown()
{
    if (!m_isInitialized) return;

    destroyAllBuffers();

    if (m_transferCommandPool != VK_NULL_HANDLE && m_device != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(m_device, m_transferCommandPool, nullptr);
        m_transferCommandPool = VK_NULL_HANDLE;
    }

    m_isInitialized = false;
    RP_LOG(BufferManagerLog, Display, "BufferManager shutdown complete");
}

std::string BufferManager::getName()
{
    return "[Buffer Manager]";
}

VkDevice BufferManager::getDevice() const
{
    return m_device;
}

bool BufferManager::initTransferQueue()
{
    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr) return false;

    // Ищем очередь, поддерживающую TRANSFER_BIT
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, queueFamilies.data());

    m_transferQueueIndex = -1;
    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            m_transferQueueIndex = i;
            break;
        }
    }

    if (m_transferQueueIndex == -1)
    {
        RP_LOG(BufferManagerLog, Warning, "No dedicated transfer queue found, using graphics queue");
        m_transferQueueIndex = deviceMgr->getGraphicsQueueIndex();
    }

    vkGetDeviceQueue(m_device, m_transferQueueIndex, 0, &m_transferQueue);

    // Создаём command pool для операций копирования
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = m_transferQueueIndex;

    if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_transferCommandPool) != VK_SUCCESS)
    {
        RP_LOG(BufferManagerLog, Error, "Failed to create transfer command pool");
        return false;
    }

    m_transferQueueInitialized = true;
    return true;
}

VkCommandBuffer BufferManager::beginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_transferCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void BufferManager::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_transferQueue);

    vkFreeCommandBuffers(m_device, m_transferCommandPool, 1, &commandBuffer);
}

uint32_t BufferManager::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    RP_LOG(BufferManagerLog, Error, "Failed to find suitable memory type");
    return 0;
}

bool BufferManager::createBuffer(const BufferCreateInfo& createInfo, BufferInfo& outBufferInfo)
{
    if (m_device == VK_NULL_HANDLE)
    {
        RP_LOG(BufferManagerLog, Error, "Device is null");
        return false;
    }

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = createInfo.size;
    bufferInfo.usage = createInfo.usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &outBufferInfo.buffer) != VK_SUCCESS)
    {
        RP_LOG(BufferManagerLog, Error, "Failed to create buffer: {}", createInfo.name);
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device, outBufferInfo.buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, createInfo.memoryProperties);

    if (vkAllocateMemory(m_device, &allocInfo, nullptr, &outBufferInfo.memory) != VK_SUCCESS)
    {
        RP_LOG(BufferManagerLog, Error, "Failed to allocate buffer memory: {}", createInfo.name);
        vkDestroyBuffer(m_device, outBufferInfo.buffer, nullptr);
        return false;
    }

    vkBindBufferMemory(m_device, outBufferInfo.buffer, outBufferInfo.memory, 0);

    outBufferInfo.size = createInfo.size;
    outBufferInfo.name = createInfo.name;

    // Если нужна host-видимая память, маппим
    if (createInfo.memoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        vkMapMemory(m_device, outBufferInfo.memory, 0, createInfo.size, 0, &outBufferInfo.mappedData);
    }

    RP_LOG(BufferManagerLog, Display, "Created buffer: {} (size: {} bytes)", createInfo.name, createInfo.size);
    return true;
}

bool BufferManager::createUniformBuffer(const std::string& name, VkDeviceSize size, BufferInfo& outBufferInfo)
{
    BufferCreateInfo createInfo{};
    createInfo.name = name;
    createInfo.size = size;
    createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    createInfo.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    return createBuffer(createInfo, outBufferInfo);
}

bool BufferManager::createVertexBuffer(const std::string& name, VkDeviceSize size, const void* data, BufferInfo& outBufferInfo)
{
    if (!m_transferQueueInitialized)
    {
        // Если нет transfer queue, создаём напрямую
        BufferCreateInfo createInfo{};
        createInfo.name = name;
        createInfo.size = size;
        createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        createInfo.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        if (!createBuffer(createInfo, outBufferInfo))
        {
            return false;
        }

        if (data)
        {
            memcpy(outBufferInfo.mappedData, data, size);
        }
        return true;
    }

    // Создаём staging buffer
    BufferInfo stagingBuffer;
    BufferCreateInfo stagingInfo{};
    stagingInfo.name = name + "_staging";
    stagingInfo.size = size;
    stagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingInfo.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    if (!createBuffer(stagingInfo, stagingBuffer))
    {
        return false;
    }

    if (data)
    {
        memcpy(stagingBuffer.mappedData, data, size);
    }

    // Создаём финальный vertex buffer
    BufferCreateInfo createInfo{};
    createInfo.name = name;
    createInfo.size = size;
    createInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    createInfo.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    if (!createBuffer(createInfo, outBufferInfo))
    {
        vkDestroyBuffer(m_device, stagingBuffer.buffer, nullptr);
        vkFreeMemory(m_device, stagingBuffer.memory, nullptr);
        return false;
    }

    // Копируем данные
    copyBuffer(stagingBuffer, outBufferInfo, size);

    // Уничтожаем staging buffer
    vkDestroyBuffer(m_device, stagingBuffer.buffer, nullptr);
    vkFreeMemory(m_device, stagingBuffer.memory, nullptr);

    return true;
}

bool BufferManager::createIndexBuffer(const std::string& name, VkDeviceSize size, const void* data, BufferInfo& outBufferInfo)
{
    if (!m_transferQueueInitialized)
    {
        BufferCreateInfo createInfo{};
        createInfo.name = name;
        createInfo.size = size;
        createInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        createInfo.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        if (!createBuffer(createInfo, outBufferInfo))
        {
            return false;
        }

        if (data)
        {
            memcpy(outBufferInfo.mappedData, data, size);
        }
        return true;
    }

    // Создаём staging buffer
    BufferInfo stagingBuffer;
    BufferCreateInfo stagingInfo{};
    stagingInfo.name = name + "_staging";
    stagingInfo.size = size;
    stagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingInfo.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    if (!createBuffer(stagingInfo, stagingBuffer))
    {
        return false;
    }

    if (data)
    {
        memcpy(stagingBuffer.mappedData, data, size);
    }

    // Создаём финальный index buffer
    BufferCreateInfo createInfo{};
    createInfo.name = name;
    createInfo.size = size;
    createInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    createInfo.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    if (!createBuffer(createInfo, outBufferInfo))
    {
        vkDestroyBuffer(m_device, stagingBuffer.buffer, nullptr);
        vkFreeMemory(m_device, stagingBuffer.memory, nullptr);
        return false;
    }

    copyBuffer(stagingBuffer, outBufferInfo, size);

    vkDestroyBuffer(m_device, stagingBuffer.buffer, nullptr);
    vkFreeMemory(m_device, stagingBuffer.memory, nullptr);

    return true;
}

bool BufferManager::createStorageBuffer(const std::string& name, VkDeviceSize size, BufferInfo& outBufferInfo)
{
    BufferCreateInfo createInfo{};
    createInfo.name = name;
    createInfo.size = size;
    createInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    createInfo.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    return createBuffer(createInfo, outBufferInfo);
}

bool BufferManager::copyBuffer(BufferInfo& src, BufferInfo& dst, VkDeviceSize size)
{
    if (!m_transferQueueInitialized)
    {
        RP_LOG(BufferManagerLog, Error, "Transfer queue not initialized");
        return false;
    }

    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, src.buffer, dst.buffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);

    return true;
}

bool BufferManager::updateBufferData(const std::string& name, const void* data, VkDeviceSize size, VkDeviceSize offset)
{
    BufferInfo* buffer = getBuffer(name);
    if (!buffer)
    {
        RP_LOG(BufferManagerLog, Error, "Buffer not found: {}", name);
        return false;
    }

    return updateBufferData(*buffer, data, size, offset);
}

bool BufferManager::updateBufferData(BufferInfo& bufferInfo, const void* data, VkDeviceSize size, VkDeviceSize offset)
{
    if (!data) return false;

    if (bufferInfo.mappedData)
    {
        // Прямое обновление
        memcpy(static_cast<char*>(bufferInfo.mappedData) + offset, data, size);
        return true;
    }
    else if (m_transferQueueInitialized)
    {
        // Нужно создать staging buffer
        BufferInfo stagingBuffer;
        BufferCreateInfo stagingInfo{};
        stagingInfo.name = bufferInfo.name + "_temp_staging";
        stagingInfo.size = size;
        stagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingInfo.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        if (!createBuffer(stagingInfo, stagingBuffer))
        {
            return false;
        }

        memcpy(stagingBuffer.mappedData, data, size);

        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = offset;
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, stagingBuffer.buffer, bufferInfo.buffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);

        vkDestroyBuffer(m_device, stagingBuffer.buffer, nullptr);
        vkFreeMemory(m_device, stagingBuffer.memory, nullptr);

        return true;
    }

    RP_LOG(BufferManagerLog, Error, "Cannot update buffer: {}", bufferInfo.name);
    return false;
}

BufferInfo* BufferManager::getBuffer(const std::string& name)
{
    auto it = m_buffers.find(name);
    if (it != m_buffers.end())
    {
        return &it->second;
    }
    return nullptr;
}

const BufferInfo* BufferManager::getBuffer(const std::string& name) const
{
    auto it = m_buffers.find(name);
    if (it != m_buffers.end())
    {
        return &it->second;
    }
    return nullptr;
}

VkBuffer BufferManager::getBufferHandle(const std::string& name) const
{
    auto it = m_buffers.find(name);
    if (it != m_buffers.end())
    {
        return it->second.buffer;
    }
    return VK_NULL_HANDLE;
}

void BufferManager::destroyBuffer(const std::string& name)
{
    auto it = m_buffers.find(name);
    if (it != m_buffers.end())
    {
        if (it->second.mappedData)
        {
            vkUnmapMemory(m_device, it->second.memory);
        }
        if (it->second.buffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(m_device, it->second.buffer, nullptr);
        }
        if (it->second.memory != VK_NULL_HANDLE)
        {
            vkFreeMemory(m_device, it->second.memory, nullptr);
        }
        m_buffers.erase(it);
        RP_LOG(BufferManagerLog, Display, "Destroyed buffer: {}", name);
    }
}

void BufferManager::destroyAllBuffers()
{
    for (auto& [name, buffer] : m_buffers)
    {
        if (buffer.mappedData)
        {
            vkUnmapMemory(m_device, buffer.memory);
        }
        if (buffer.buffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(m_device, buffer.buffer, nullptr);
        }
        if (buffer.memory != VK_NULL_HANDLE)
        {
            vkFreeMemory(m_device, buffer.memory, nullptr);
        }
    }
    m_buffers.clear();
    RP_LOG(BufferManagerLog, Display, "Destroyed all buffers");
}

void BufferManager::cleanup()
{
    destroyAllBuffers();
}