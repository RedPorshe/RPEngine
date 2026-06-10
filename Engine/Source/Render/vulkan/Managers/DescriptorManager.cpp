#include "DescriptorManager.h"
#include "../vkContext.h"
#include "DeviceManager.h"
#include "Log/Log.h"

using namespace RPE;

DEFINE_LOG_CATEGORY_STATIC(DescriptorManagerLog);

DescriptorManager::DescriptorManager() {}

DescriptorManager::~DescriptorManager()
{
    shutdown();
}

bool DescriptorManager::preInit(const WindowSettings& settings, const std::string& EngineName)
{
    return true;
}

bool DescriptorManager::init()
{
    if (!m_contextPtr)
    {
        RP_LOG(DescriptorManagerLog, Error, "Context ptr is null");
        return false;
    }

    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr)
    {
        RP_LOG(DescriptorManagerLog, Error, "Device manager is null");
        return false;
    }

    VkDevice device = deviceMgr->getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(DescriptorManagerLog, Error, "Logical device is null");
        return false;
    }

    // Создаём стандартные пулы
    createDefaultPools();

    m_isInitialized = true;
    RP_LOG(DescriptorManagerLog, Display, "DescriptorManager initialized successfully");
    return true;
}

void DescriptorManager::registerPipelineLayouts(const std::vector<std::string>& pipelineNames)
{
    if (!m_isInitialized)
    {
        RP_LOG(DescriptorManagerLog, Error, "DescriptorManager not initialized");
        return;
    }

    if (pipelineNames.empty())
    {
        RP_LOG(DescriptorManagerLog, Warning, "No pipeline names provided");
        return;
    }

    // Создаём layout для каждого пайплайна индивидуально
    for (const auto& pipelineName : pipelineNames)
    {
        if (!hasDescriptorSetLayout(pipelineName))
        {
            if (createPipelineDescriptorSetLayout(pipelineName))
            {
                RP_LOG(DescriptorManagerLog, Display, "Created descriptor set layout for pipeline: {}", pipelineName);
            }
            else
            {
                RP_LOG(DescriptorManagerLog, Error, "Failed to create layout for pipeline: {}", pipelineName);
            }
        }
    }
}

bool RPE::DescriptorManager::createPipelineDescriptorSetLayout(const std::string& pipelineName)
{
    if (hasDescriptorSetLayout(pipelineName))
    {
        return true;
    }

    std::vector<VkDescriptorSetLayoutBinding> bindings;

    if (pipelineName == "main" || pipelineName == "Triangle" || pipelineName == "Wireframe" || pipelineName == "PointCloud")
    {
        // Только MVP матрицы
        VkDescriptorSetLayoutBinding mvpBinding{};
        mvpBinding.binding = 0;
        mvpBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        mvpBinding.descriptorCount = 1;
        mvpBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        bindings.push_back(mvpBinding);
    }
    else if (pipelineName == "Transparent")
    {
        // MVP матрицы + материал (цвет, прозрачность)
        VkDescriptorSetLayoutBinding mvpBinding{};
        mvpBinding.binding = 0;
        mvpBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        mvpBinding.descriptorCount = 1;
        mvpBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        bindings.push_back(mvpBinding);

        VkDescriptorSetLayoutBinding materialBinding{};
        materialBinding.binding = 1;
        materialBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        materialBinding.descriptorCount = 1;
        materialBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings.push_back(materialBinding);
    }

    return createDescriptorSetLayout(pipelineName, bindings);
}

void DescriptorManager::createDefaultPools()
{

    std::vector<VkDescriptorPoolSize> poolSizes;

    VkDescriptorPoolSize uboPoolSize{};
    uboPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboPoolSize.descriptorCount = 200;
    poolSizes.push_back(uboPoolSize);

    VkDescriptorPoolSize sboPoolSize{};
    sboPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    sboPoolSize.descriptorCount = 50;
    poolSizes.push_back(sboPoolSize);

    VkDescriptorPoolSize samplerPoolSize{};
    samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerPoolSize.descriptorCount = 50;
    poolSizes.push_back(samplerPoolSize);

    if (!hasDescriptorPool("MainPool"))
    {
        if (createDescriptorPool("MainPool", poolSizes, 300))
        {
            RP_LOG(DescriptorManagerLog, Display, "Created main descriptor pool");
        }
        else
        {
            RP_LOG(DescriptorManagerLog, Error, "Failed to create main descriptor pool");
        }
    }
}

void DescriptorManager::shutdown()
{
    if (!m_isInitialized) return;
    cleanup();
    m_isInitialized = false;
    RP_LOG(DescriptorManagerLog, Display, "DescriptorManager shutdown complete");
}

std::string DescriptorManager::getName()
{
    return "[Descriptor Manager]";
}

VkDevice DescriptorManager::getDevice() const
{
    if (!m_contextPtr)
    {
        RP_LOG(DescriptorManagerLog, Error, "Context ptr is null");
        return VK_NULL_HANDLE;
    }

    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr)
    {
        RP_LOG(DescriptorManagerLog, Error, "Device manager is null");
        return VK_NULL_HANDLE;
    }

    return deviceMgr->getDevice();
}

bool DescriptorManager::createDescriptorSetLayout(const std::string& name, const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
    if (hasDescriptorSetLayout(name))
    {
        RP_LOG(DescriptorManagerLog, Warning, "Descriptor set layout '{}' already exists", name);
        return true;
    }

    VkDevice device = getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(DescriptorManagerLog, Error, "Failed to get device");
        return false;
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    VkDescriptorSetLayout layout;
    VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout);
    if (result != VK_SUCCESS)
    {
        RP_LOG(DescriptorManagerLog, Error, "Failed to create descriptor set layout '{}': {}", name, static_cast<int>(result));
        return false;
    }

    m_layouts[name] = layout;
    RP_LOG(DescriptorManagerLog, Display, "Created descriptor set layout '{}' with {} bindings", name, bindings.size());
    return true;
}

VkDescriptorSetLayout DescriptorManager::getDescriptorSetLayout(const std::string& name) const
{
    auto it = m_layouts.find(name);
    if (it == m_layouts.end())
    {
        RP_LOG(DescriptorManagerLog, Warning, "Descriptor set layout '{}' not found", name);
        return VK_NULL_HANDLE;
    }
    return it->second;
}

bool DescriptorManager::hasDescriptorSetLayout(const std::string& name) const
{
    return m_layouts.find(name) != m_layouts.end();
}

bool DescriptorManager::createDescriptorPool(const std::string& name, const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets)
{
    if (hasDescriptorPool(name))
    {
        RP_LOG(DescriptorManagerLog, Warning, "Descriptor pool '{}' already exists", name);
        return true;
    }

    VkDevice device = getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(DescriptorManagerLog, Error, "Failed to get device");
        return false;
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxSets;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    VkDescriptorPool pool;
    VkResult result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool);
    if (result != VK_SUCCESS)
    {
        RP_LOG(DescriptorManagerLog, Error, "Failed to create descriptor pool '{}': {}", name, static_cast<int>(result));
        return false;
    }

    m_pools[name] = pool;
    RP_LOG(DescriptorManagerLog, Display, "Created descriptor pool '{}' with {} max sets", name, maxSets);
    return true;
}

VkDescriptorPool DescriptorManager::getDescriptorPool(const std::string& name) const
{
    auto it = m_pools.find(name);
    if (it == m_pools.end())
    {
        RP_LOG(DescriptorManagerLog, Warning, "Descriptor pool '{}' not found", name);
        return VK_NULL_HANDLE;
    }
    return it->second;
}

bool DescriptorManager::hasDescriptorPool(const std::string& name) const
{
    return m_pools.find(name) != m_pools.end();
}

bool DescriptorManager::allocateDescriptorSet(const std::string& poolName, const std::string& layoutName, VkDescriptorSet& outSet)
{
    VkDescriptorPool pool = getDescriptorPool(poolName);
    VkDescriptorSetLayout layout = getDescriptorSetLayout(layoutName);

    if (pool == VK_NULL_HANDLE)
    {
        RP_LOG(DescriptorManagerLog, Error, "Pool '{}' not found", poolName);
        return false;
    }

    if (layout == VK_NULL_HANDLE)
    {
        RP_LOG(DescriptorManagerLog, Error, "Layout '{}' not found", layoutName);
        return false;
    }

    VkDevice device = getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(DescriptorManagerLog, Error, "Failed to get device");
        return false;
    }

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    VkResult result = vkAllocateDescriptorSets(device, &allocInfo, &outSet);
    if (result != VK_SUCCESS)
    {
        RP_LOG(DescriptorManagerLog, Error, "Failed to allocate descriptor set: {}", static_cast<int>(result));
        return false;
    }

    return true;
}

void DescriptorManager::freeDescriptorSet(const std::string& poolName, VkDescriptorSet set)
{
    VkDescriptorPool pool = getDescriptorPool(poolName);
    if (pool == VK_NULL_HANDLE)
    {
        RP_LOG(DescriptorManagerLog, Warning, "Pool '{}' not found, cannot free descriptor set", poolName);
        return;
    }

    VkDevice device = getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(DescriptorManagerLog, Error, "Failed to get device");
        return;
    }

    vkFreeDescriptorSets(device, pool, 1, &set);
}

void DescriptorManager::updateUniformBufferDescriptor(
    VkDescriptorSet descriptorSet, uint32_t binding, VkBuffer buffer, VkDeviceSize size, uint32_t offset)
{
    VkDevice device = getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(DescriptorManagerLog, Error, "Failed to get device");
        return;
    }

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer;
    bufferInfo.offset = offset;
    bufferInfo.range = size;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptorSet;
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

void DescriptorManager::updateCombinedImageDescriptor(
    VkDescriptorSet descriptorSet, uint32_t binding, VkImageView imageView, VkSampler sampler)
{
    VkDevice device = getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(DescriptorManagerLog, Error, "Failed to get device");
        return;
    }

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = imageView;
    imageInfo.sampler = sampler;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptorSet;
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

void DescriptorManager::updateStorageBufferDescriptor(
    VkDescriptorSet descriptorSet, uint32_t binding, VkBuffer buffer, VkDeviceSize size, uint32_t offset)
{
    VkDevice device = getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(DescriptorManagerLog, Error, "Failed to get device");
        return;
    }

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer;
    bufferInfo.offset = offset;
    bufferInfo.range = size;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptorSet;
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

void DescriptorManager::cleanup()
{
    VkDevice device = getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(DescriptorManagerLog, Warning, "Cannot cleanup: device is null");
        m_layouts.clear();
        m_pools.clear();
        return;
    }

    // Уничтожаем пулы
    for (auto& [name, pool] : m_pools)
    {
        if (pool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(device, pool, nullptr);
            RP_LOG(DescriptorManagerLog, Display, "Destroyed descriptor pool '{}'", name);
        }
    }
    m_pools.clear();

    // Уничтожаем layouts
    for (auto& [name, layout] : m_layouts)
    {
        if (layout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, layout, nullptr);
            RP_LOG(DescriptorManagerLog, Display, "Destroyed descriptor set layout '{}'", name);
        }
    }
    m_layouts.clear();
}