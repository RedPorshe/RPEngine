#pragma once
#include "IVkManager.h"
#include "Window/IWindow.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <vulkan/vulkan.h>

namespace RPE
{
class DescriptorManager : public IVkManager
{
public:
    DescriptorManager();
    ~DescriptorManager() override;

    bool preInit(const WindowSettings& settings, const std::string& EngineName) override;
    bool init() override;
    void shutdown() override;
    std::string getName() override;

    void registerPipelineLayouts(const std::vector<std::string>& pipelineNames);
    bool createPipelineDescriptorSetLayout(const std::string& pipelineName);

    bool createDescriptorSetLayout(const std::string& name, const std::vector<VkDescriptorSetLayoutBinding>& bindings);
    VkDescriptorSetLayout getDescriptorSetLayout(const std::string& name) const;
    bool hasDescriptorSetLayout(const std::string& name) const;

    bool createDescriptorPool(const std::string& name, const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets);
    VkDescriptorPool getDescriptorPool(const std::string& name) const;
    bool hasDescriptorPool(const std::string& name) const;

    bool allocateDescriptorSet(const std::string& poolName, const std::string& layoutName, VkDescriptorSet& outSet);
    void freeDescriptorSet(const std::string& poolName, VkDescriptorSet set);

    void updateUniformBufferDescriptor(
        VkDescriptorSet descriptorSet, uint32_t binding, VkBuffer buffer, VkDeviceSize size, uint32_t offset = 0);
    void updateCombinedImageDescriptor(VkDescriptorSet descriptorSet, uint32_t binding, VkImageView imageView, VkSampler sampler);
    void updateStorageBufferDescriptor(
        VkDescriptorSet descriptorSet, uint32_t binding, VkBuffer buffer, VkDeviceSize size, uint32_t offset = 0);

private:
    void cleanup();
    void createDefaultPools();
    VkDevice getDevice() const;

    std::unordered_map<std::string, VkDescriptorSetLayout> m_layouts;
    std::unordered_map<std::string, VkDescriptorPool> m_pools;
};
}  // namespace RPE