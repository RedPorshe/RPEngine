#pragma once
#include "IVkManager.h"
#include <unordered_map>
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace RPE
{
    struct PipelineCreateInfo
        {
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        VkPipelineViewportStateCreateInfo viewportState{};
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        VkPipelineMultisampleStateCreateInfo multisampling{};
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        std::vector<VkDynamicState> dynamicStates;
        VkPipelineDynamicStateCreateInfo dynamicState{};
        };
struct PipelineInfo
{
    VkPipeline pipeline{VK_NULL_HANDLE};
    VkPipelineLayout layout{VK_NULL_HANDLE};
    VkPipelineCache cache{VK_NULL_HANDLE};
    std::string name{};
};

class PipelineManager : public IVkManager
{
public:
    PipelineManager();
    ~PipelineManager() override;

    bool preInit(const WindowSettings& settings, const std::string& EngineName) override;
    bool init() override;
    void shutdown() override;
    std::string getName() override;
    VkPipeline getPipeline(const std::string_view name) const;
    VkPipelineLayout getLayout(const std::string_view name) const;

private:
    bool CreatePipelines();
    bool CreateGraphicsPipeline(const std::string name);
    VkPipelineLayout createPipelineLayout();
    std::vector<std::string> m_availablePipelines;
    std::unordered_map<std::string, PipelineInfo> m_pipelines;
};
}  // namespace RPE