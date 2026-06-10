#include "PipelineManager.h"
#include "ShadersManager.h"
#include "RenderpassManager.h"
#include "DescriptorManager.h"
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

    auto* descMgr = m_contextPtr->getDescriptorManager();
    if (!descMgr || !descMgr->isInitialized())
    {
        RP_LOG(PipelineLog, Error, "DescriptorManager is not initialized");
        return false;
    }
    descMgr->registerPipelineLayouts(m_availablePipelines);
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
            RP_LOG(PipelineLog, Display, "Destroyed layout: {}", name);
        }
        if (info.cache != VK_NULL_HANDLE)
        {
            vkDestroyPipelineCache(device, info.cache, nullptr);
            RP_LOG(PipelineLog, Display, "Destroyed cache: {}", name);
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

bool PipelineManager::CreateGraphicsPipeline(const std::string name)
{
    RP_LOG(PipelineLog, Display, "Creating pipeline: {}", name);

    if (!m_contextPtr)
    {
        RP_LOG(PipelineLog, Error, "Vulkan context is null");
        return false;
    }

    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr)
    {
        RP_LOG(PipelineLog, Error, "Device manager is null");
        return false;
    }

    VkDevice device = deviceMgr->getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(PipelineLog, Error, "Logical Device is null");
        return false;
    }

    VkResult result{};

    // Создаём pipeline cache
    VkPipelineCache pipelinecache = VK_NULL_HANDLE;
    VkPipelineCacheCreateInfo cachecreateInfo{};
    cachecreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    cachecreateInfo.pNext = nullptr;
    cachecreateInfo.initialDataSize = 0;
    cachecreateInfo.pInitialData = nullptr;
    cachecreateInfo.flags = 0;

    result = vkCreatePipelineCache(device, &cachecreateInfo, nullptr, &pipelinecache);
    if (result != VK_SUCCESS)
    {
        RP_LOG(PipelineLog, Error, "Failed to create pipeline cache");
        return false;
    }
    RP_LOG(PipelineLog, Display, "Pipeline cache created for pipeline: {}", name);

    PipelineCreateInfo pipelineCreateData{};
    pipelineCreateData.pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateData.pipelineInfo.pNext = nullptr;
    pipelineCreateData.pipelineInfo.flags = 0;

    auto* shaderMgr = m_contextPtr->getShaderManager();
    if (!shaderMgr)
    {

        RP_LOG(PipelineLog, Error, "ShaderManager is null ");
        return false;
    }
    auto* shadersInfo = shaderMgr->getShaders(name);
    if (!shadersInfo)
    {
        RP_LOG(PipelineLog, Error, "Shaders not found for pipeline: {}", name);
        return false;
    }

    for (const auto& shaderInfo : shadersInfo->shaders)
    {
        VkPipelineShaderStageCreateInfo stage{};
        stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stage.stage = static_cast<VkShaderStageFlagBits>(shaderInfo.stage);
        stage.module = shaderInfo.module;
        stage.pName = shaderInfo.entryPoint.c_str();
        pipelineCreateData.shaderStages.push_back(stage);
    }

    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
    VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
    VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    float lineWidth = 1.0f;
    bool bPrimitiveRestartEnable = false;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    VkBool32 blendEnable = VK_FALSE;
    VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    VkBlendOp colorBlendOp = VK_BLEND_OP_ADD;
    VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD;
    VkColorComponentFlags colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    // vertex
    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

    if (name == "main")
    {
        topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        polygonMode = VK_POLYGON_MODE_FILL;
        cullMode = VK_CULL_MODE_BACK_BIT;
        frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }
    else if (name == "Triangle")
    {
        topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        polygonMode = VK_POLYGON_MODE_FILL;
        cullMode = VK_CULL_MODE_NONE;
        frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }
    else if (name == "Wireframe")
    {
        topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        polygonMode = VK_POLYGON_MODE_LINE;
        cullMode = VK_CULL_MODE_NONE;
        lineWidth = 2.0f;
    }
    else if (name == "PointCloud")
    {
        topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        polygonMode = VK_POLYGON_MODE_POINT;
        cullMode = VK_CULL_MODE_NONE;
    }
    else if (name == "Transparent")
    {
        blendEnable = VK_TRUE;
        srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    }

    pipelineCreateData.pipelineInfo.stageCount = static_cast<uint32_t>(pipelineCreateData.shaderStages.size());
    pipelineCreateData.pipelineInfo.pStages = pipelineCreateData.shaderStages.data();

    // Vertex input
    pipelineCreateData.vertexInputInfo = {};
    pipelineCreateData.vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineCreateData.vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
    pipelineCreateData.vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    pipelineCreateData.vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
    pipelineCreateData.vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
    pipelineCreateData.pipelineInfo.pVertexInputState = &pipelineCreateData.vertexInputInfo;

    // Input assembly
    pipelineCreateData.inputAssembly = {};
    pipelineCreateData.inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineCreateData.inputAssembly.topology = topology;
    pipelineCreateData.inputAssembly.primitiveRestartEnable = bPrimitiveRestartEnable ? VK_TRUE : VK_FALSE;
    pipelineCreateData.pipelineInfo.pInputAssemblyState = &pipelineCreateData.inputAssembly;

    // Viewport state
    pipelineCreateData.viewportState = {};
    pipelineCreateData.viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipelineCreateData.viewportState.viewportCount = 1;
    pipelineCreateData.viewportState.scissorCount = 1;
    pipelineCreateData.pipelineInfo.pViewportState = &pipelineCreateData.viewportState;

    // Rasterization
    pipelineCreateData.rasterizer = {};
    pipelineCreateData.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pipelineCreateData.rasterizer.polygonMode = polygonMode;
    pipelineCreateData.rasterizer.cullMode = cullMode;
    pipelineCreateData.rasterizer.frontFace = frontFace;
    pipelineCreateData.rasterizer.lineWidth = lineWidth;
    pipelineCreateData.pipelineInfo.pRasterizationState = &pipelineCreateData.rasterizer;

    // Multisample
    pipelineCreateData.multisampling = {};
    pipelineCreateData.multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pipelineCreateData.multisampling.rasterizationSamples = samples;
    pipelineCreateData.pipelineInfo.pMultisampleState = &pipelineCreateData.multisampling;

    // Color blend attachment
    pipelineCreateData.colorBlendAttachment = {};
    pipelineCreateData.colorBlendAttachment.colorWriteMask = colorWriteMask;
    pipelineCreateData.colorBlendAttachment.blendEnable = blendEnable;
    pipelineCreateData.colorBlendAttachment.srcColorBlendFactor = srcColorBlendFactor;
    pipelineCreateData.colorBlendAttachment.dstColorBlendFactor = dstColorBlendFactor;
    pipelineCreateData.colorBlendAttachment.colorBlendOp = colorBlendOp;
    pipelineCreateData.colorBlendAttachment.srcAlphaBlendFactor = srcAlphaBlendFactor;
    pipelineCreateData.colorBlendAttachment.dstAlphaBlendFactor = dstAlphaBlendFactor;
    pipelineCreateData.colorBlendAttachment.alphaBlendOp = alphaBlendOp;

    // Color blend state
    pipelineCreateData.colorBlending = {};
    pipelineCreateData.colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pipelineCreateData.colorBlending.attachmentCount = 1;
    pipelineCreateData.colorBlending.pAttachments = &pipelineCreateData.colorBlendAttachment;
    pipelineCreateData.pipelineInfo.pColorBlendState = &pipelineCreateData.colorBlending;

    // Dynamic states
    pipelineCreateData.dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    pipelineCreateData.dynamicState = {};
    pipelineCreateData.dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pipelineCreateData.dynamicState.pNext = nullptr;
    pipelineCreateData.dynamicState.flags = 0;
    pipelineCreateData.dynamicState.dynamicStateCount = static_cast<uint32_t>(pipelineCreateData.dynamicStates.size());
    pipelineCreateData.dynamicState.pDynamicStates = pipelineCreateData.dynamicStates.data();
    pipelineCreateData.pipelineInfo.pDynamicState = &pipelineCreateData.dynamicState;

    // Render pass
    auto* renderPassMgr = m_contextPtr->getRenderpassManager();
    pipelineCreateData.pipelineInfo.renderPass = renderPassMgr->getRenderPass();
    pipelineCreateData.pipelineInfo.subpass = 0;

    // Создаём layout
    VkPipelineLayout layout = createPipelineLayout(name);
    if (layout == VK_NULL_HANDLE) return false;
    pipelineCreateData.pipelineInfo.layout = layout;

    // Создаём пайплайн
    VkPipeline pipeline = VK_NULL_HANDLE;
    result = vkCreateGraphicsPipelines(device, pipelinecache, 1, &pipelineCreateData.pipelineInfo, nullptr, &pipeline);

    if (result != VK_SUCCESS)
    {
        RP_LOG(PipelineLog, Error, "Failed to create pipeline '{}'. error code: {}", name, static_cast<int>(result));
        return false;
    }

    m_pipelines[name].cache = pipelinecache;
    m_pipelines[name].name = name;
    m_pipelines[name].layout = layout;
    m_pipelines[name].pipeline = pipeline;
    RP_LOG(PipelineLog, Display, "Pipeline '{}' created successfully", name);
    return true;
}

VkPipelineLayout PipelineManager::createPipelineLayout(const std::string& name)
{
    auto* descMgr = m_contextPtr->getDescriptorManager();
    if (!descMgr)
    {
        RP_LOG(PipelineLog, Error, "DescriptorManager is null");
        return VK_NULL_HANDLE;
    }

    VkDescriptorSetLayout descLayout = descMgr->getDescriptorSetLayout(name);

    if (descLayout == VK_NULL_HANDLE)
    {
        RP_LOG(PipelineLog, Warning, "No specific layout for '{}', trying shared layout", name);
        descLayout = descMgr->getDescriptorSetLayout("StandardGraphicsLayout");

        if (descLayout == VK_NULL_HANDLE)
        {
            RP_LOG(PipelineLog, Error, "No shared layout available");
            return VK_NULL_HANDLE;
        }
    }

    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &descLayout;
    layoutInfo.pushConstantRangeCount = 0;
    layoutInfo.pPushConstantRanges = nullptr;

    VkPipelineLayout layout;
    auto* deviceMgr = m_contextPtr->getDeviceManager();
    VkDevice device = deviceMgr->getDevice();

    VkResult result = vkCreatePipelineLayout(device, &layoutInfo, nullptr, &layout);
    if (result != VK_SUCCESS)
    {
        RP_LOG(PipelineLog, Error, "Failed to create pipeline layout for '{}': {}", name, static_cast<int>(result));
        return VK_NULL_HANDLE;
    }

    RP_LOG(PipelineLog, Display, "Created pipeline layout for '{}'", name);
    return layout;
}