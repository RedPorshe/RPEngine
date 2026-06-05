#include "PipelineManager.h"
#include "DeviceManager.h"
#include "../vkContext.h"
#include "../vkRender.h"
#include "Core/Engine.h"

using namespace RPE;
DEFINE_LOG_CATEGORY_STATIC(PipelineLog);

PipelineManager::PipelineManager() {}

PipelineManager::~PipelineManager() {}

bool PipelineManager::preInit(const WindowSettings& settings, const std::string& EngineName)
{

    return true;
}

bool PipelineManager::init()
{
    if (!m_contextPtr)
    {
        RP_LOG(PipelineLog, Error, "Context ptr is null");
        return false;
    }

    auto renderer = m_contextPtr->getVulkanRenderer();
    if (!renderer)
    {
        RP_LOG(PipelineLog, Error, "Renderer ptr is null");
        return false;
    }

    auto engineptr = renderer->getEngine();
    if (!engineptr)
    {
        RP_LOG(PipelineLog, Error, "Engine ptr is null");
        return false;
    }

    const auto NeededPipelines = engineptr->getNeededPipelineNames();
    if (NeededPipelines.empty())
    {
        RP_LOG(PipelineLog, Error, "no pipeline enabled...");
        return false;
    }
    for (auto& name : NeededPipelines)
    {
        m_availablePipelines.push_back(name);
    }
    auto succes = CreatePipelines();
    if (!succes)
    {
        RP_LOG(PipelineLog, Error, "Failed to create pipelines");
        return succes;
    }

    RP_LOG(PipelineLog, Display, "Pipelines created success");
    m_isInitialized = true;
    return succes;
}

void PipelineManager::shutdown()
{
    if (!m_contextPtr) return;

    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr) return;

    VkDevice device = deviceMgr->getDevice();
    if (device == VK_NULL_HANDLE) return;

    for (auto& [name, info] : m_pipelines)
    {
        if (info.pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(device, info.pipeline, nullptr);
            RP_LOG(PipelineLog, Display, "Destroyed pipeline: {}", name);
        }
        if (info.layout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(device, info.layout, nullptr);
        }
    }
    m_pipelines.clear();
    m_availablePipelines.clear();
    m_isInitialized = false;
}

std::string PipelineManager::getName()
{
    return "[Pipeline Manager]";
}

VkPipeline PipelineManager::getPipeline(const std::string_view name) const
{
    auto it = m_pipelines.find(std::string(name));
    return it != m_pipelines.end() ? it->second.pipeline : VK_NULL_HANDLE;
}

VkPipelineLayout PipelineManager::getLayout(const std::string_view name) const
{
    auto it = m_pipelines.find(std::string(name));
    return it != m_pipelines.end() ? it->second.layout : VK_NULL_HANDLE;
}

bool PipelineManager::CreatePipelines()
{
    bool success = true;
    for (const auto& name : m_availablePipelines)
    {
        if (!CreateGraphicsPipeline(name))
        {
            success = false;
            RP_LOG(PipelineLog, Error, "Failed to create {} pipeline", name);
            break;
        }
    }
    return success;
}

bool RPE::PipelineManager::CreateGraphicsPipeline(const std::string_view name)
{
    RP_LOG(PipelineLog, Display, "Creating pipeline: {}", name);
    RP_LOG(PipelineLog, Warning, "Creating pipelines is WIP");
    return true;
}
