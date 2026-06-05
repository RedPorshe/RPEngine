#pragma once
#include "IVkManager.h"
#include <unordered_map>
#include <vulkan/vulkan.h>
#include <string_view>
#include <vector>

namespace RPE
{
struct PipelineInfo
{
    VkPipeline pipeline{VK_NULL_HANDLE};
    VkPipelineLayout layout{VK_NULL_HANDLE};
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
    bool CreateGraphicsPipeline(const std::string_view name);
    std::vector<std::string> m_availablePipelines;
    std::unordered_map<std::string, PipelineInfo> m_pipelines;
};
}  // namespace RPE