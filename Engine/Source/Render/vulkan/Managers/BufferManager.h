#pragma once
#include "IVkManager.h"
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace RPE
{
struct BufferCreateInfo
{
    VkDeviceSize size;
    VkBufferUsageFlags usage;
    VkMemoryPropertyFlags memoryProperties;
    std::string name;
};

struct BufferInfo
{
    VkBuffer buffer{VK_NULL_HANDLE};
    VkDeviceMemory memory{VK_NULL_HANDLE};
    void* mappedData{nullptr};
    VkDeviceSize size{0};
    std::string name;
};

class BufferManager : public IVkManager
{
public:
    BufferManager();
    ~BufferManager() override;

    bool preInit(const WindowSettings& settings, const std::string& EngineName) override;
    bool init() override;
    void shutdown() override;
    std::string getName() override;

    // Создание буферов
    bool createBuffer(const BufferCreateInfo& createInfo, BufferInfo& outBufferInfo);
    bool createUniformBuffer(const std::string& name, VkDeviceSize size, BufferInfo& outBufferInfo);
    bool createVertexBuffer(const std::string& name, VkDeviceSize size, const void* data, BufferInfo& outBufferInfo);
    bool createIndexBuffer(const std::string& name, VkDeviceSize size, const void* data, BufferInfo& outBufferInfo);
    bool createStorageBuffer(const std::string& name, VkDeviceSize size, BufferInfo& outBufferInfo);

    // Получение буферов
    BufferInfo* getBuffer(const std::string& name);
    const BufferInfo* getBuffer(const std::string& name) const;
    VkBuffer getBufferHandle(const std::string& name) const;

    // Обновление данных
    bool updateBufferData(const std::string& name, const void* data, VkDeviceSize size, VkDeviceSize offset = 0);
    bool updateBufferData(BufferInfo& bufferInfo, const void* data, VkDeviceSize size, VkDeviceSize offset = 0);

    // Копирование между буферами
    bool copyBuffer(BufferInfo& src, BufferInfo& dst, VkDeviceSize size);

    // Удаление буферов
    void destroyBuffer(const std::string& name);
    void destroyAllBuffers();

    // Вспомогательные методы
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkDevice getDevice() const;

private:
    void cleanup();

    std::unordered_map<std::string, BufferInfo> m_buffers;
    VkPhysicalDevice m_physicalDevice{VK_NULL_HANDLE};
    VkDevice m_device{VK_NULL_HANDLE};
    VkQueue m_transferQueue{VK_NULL_HANDLE};
    int m_transferQueueIndex{-1};

    bool m_transferQueueInitialized{false};

    // Вспомогательный command pool для операций копирования
    VkCommandPool m_transferCommandPool{VK_NULL_HANDLE};

    bool initTransferQueue();
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
};
}  // namespace RPE