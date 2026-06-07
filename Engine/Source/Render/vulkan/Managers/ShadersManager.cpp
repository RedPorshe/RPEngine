#include "ShadersManager.h"
#include "Log/Log.h"
#include "DeviceManager.h"
#include "../vkContext.h"
#include "../vkRender.h"
#include "Core/Engine.h"
#include <fstream>
#include <filesystem>

DEFINE_LOG_CATEGORY_STATIC(ShaderLog);

using namespace RPE;

ShaderManager::ShaderManager()
{
    RP_LOG(ShaderLog, Display, "{} created", getName());
}

ShaderManager::~ShaderManager()
{
    shutdown();
}

bool ShaderManager::preInit(const WindowSettings& settings, const std::string& EngineName)
{
    return true;
}

bool ShaderManager::init()
{
    if (!m_contextPtr)
    {
        RP_LOG(ShaderLog, Error, "Context ptr is null");
        return false;
    }

    auto* renderer = m_contextPtr->getVulkanRenderer();
    if (!renderer)
    {
        RP_LOG(ShaderLog, Error, "Renderer ptr is null");
        return false;
    }

    auto* engine = renderer->getEngine();
    if (!engine)
    {
        RP_LOG(ShaderLog, Error, "Engine ptr is null");
        return false;
    }

    const auto pipelineNames = engine->getNeededPipelineNames();

    const std::string assetspath = engine->getAssetsPath();
    RP_LOG(ShaderLog, Display, "Assets path: {} ", assetspath);
    for (const auto& name : pipelineNames)
    {

        ShadersInfo info;
        info.pipelineName = name;
        info.shaders = {
            // Графические шейдеры
            {ShaderStage::Vertex,assetspath + "shaders/" + name + "_vert.spv", assetspath + "shaders/" + name + ".vert", "main"},
            {ShaderStage::Fragment,assetspath + "shaders/" + name + "_frag.spv", assetspath + "shaders/" + name + ".frag", "main"},

            // Опциональные графические шейдеры (если нужны)
            // {ShaderStage::Geometry, "shaders/" + name + "_geom.spv", "main"},
            // {ShaderStage::TessellationControl, "shaders/" + name + "_tesc.spv", "main"},
            // {ShaderStage::TessellationEvaluation, "shaders/" + name + "_tese.spv", "main"},

            // Вычислительный шейдер (если нужен)
            // {ShaderStage::Compute, "shaders/" + name + "_comp.spv", "main"},
        };
        m_shadersInfos[name] = info;
        RP_LOG(ShaderLog, Display, "Registered shaders info for pipeline: {}", name);
    }

    if (!compileShaders())
    {
        RP_LOG(ShaderLog, Error, "Failed to compile modules");
        return false;
    }
    if (!createShaderModules())
    {
        RP_LOG(ShaderLog, Error, "Failed to create shader modules");
        return false;
    }

    m_isInitialized = true;
    RP_LOG(ShaderLog, Display, "ShaderManager initialized successfully with {} pipelines", m_shadersInfos.size());
    return true;
}

void ShaderManager::shutdown()
{
    if (!m_isInitialized) return;

    if (!m_contextPtr)
    {
        m_isInitialized = false;
        return;
    }

    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr)
    {
        m_isInitialized = false;
        return;
    }

    VkDevice device = deviceMgr->getDevice();
    if (device == VK_NULL_HANDLE)
    {
        m_isInitialized = false;
        return;
    }

    // Уничтожаем все шейдерные модули
    for (auto& [name, info] : m_shadersInfos)
    {
        for (auto& shaderInfo : info.shaders)
        {
            if (shaderInfo.module != VK_NULL_HANDLE)
            {
                vkDestroyShaderModule(device, shaderInfo.module, nullptr);
                RP_LOG(ShaderLog, Display, "Destroyed shader module for pipeline: {}, stage: {}", name, static_cast<int>(shaderInfo.stage));
            }
        }
    }

    m_shadersInfos.clear();
    m_isInitialized = false;
    RP_LOG(ShaderLog, Display, "ShaderManager shutdown complete");
}

std::string ShaderManager::getName()
{
    return "[Shader Manager]";
}

ShadersInfo* ShaderManager::getShaders(const std::string& pipelineName)
{
    auto it = m_shadersInfos.find(pipelineName);
    if (it != m_shadersInfos.end())
    {
        return &it->second;
    }
    RP_LOG(ShaderLog, Warning, "Shaders not found for pipeline: {}", pipelineName);
    return nullptr;
}

bool ShaderManager::hasShaders(const std::string& pipelineName) const
{
    return m_shadersInfos.find(pipelineName) != m_shadersInfos.end();
}

bool ShaderManager::loadShaderModule(ShaderModuleInfo& shaderInfo)
{

    if (!std::filesystem::exists(shaderInfo.path))
    {
        RP_LOG(ShaderLog, Error, "Shader file not found: {}", shaderInfo.path);
        return false;
    }

    // Загружаем бинарные данные .spv файла
    std::ifstream file(shaderInfo.path, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
        RP_LOG(ShaderLog, Error, "Failed to open shader file: {}", shaderInfo.path);
        return false;
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    if (buffer.empty())
    {
        RP_LOG(ShaderLog, Error, "Shader file is empty: {}", shaderInfo.path);
        return false;
    }

    
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = buffer.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());
    

    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr)
    {
        RP_LOG(ShaderLog, Error, "Device manager is null");
        return false;
    }

    VkDevice device = deviceMgr->getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(ShaderLog, Error, "Device is null");
        return false;
    }

    VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderInfo.module);
    if (result != VK_SUCCESS)
    {
        RP_LOG(ShaderLog, Error, "Failed to create shader module from {}: {}", shaderInfo.path, static_cast<int>(result));
        return false;
    }
    std::string stageStr = stageToString(shaderInfo.stage);
    RP_LOG(ShaderLog, Display, "Loaded shader module: {} (stage: {})", shaderInfo.path,stageStr);

    return true;
}

bool ShaderManager::createShaderModules()
{
    if (!m_contextPtr)
    {
        RP_LOG(ShaderLog, Error, "Context ptr is null");
        return false;
    }

    auto* deviceMgr = m_contextPtr->getDeviceManager();
    if (!deviceMgr)
    {
        RP_LOG(ShaderLog, Error, "Device manager is null");
        return false;
    }

    VkDevice device = deviceMgr->getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(ShaderLog, Error, "Device is null");
        return false;
    }

    for (auto& [name, info] : m_shadersInfos)
    {
        for (auto& shaderInfo : info.shaders)
        {
            if (!loadShaderModule(shaderInfo))
            {
                std::string stageStr = stageToString(shaderInfo.stage);
                RP_LOG(ShaderLog, Error, "Failed to load shader module for pipeline: {}, stage: {}", name, stageStr);
                return false;
            }
        }
    }

    return true;
}

bool ShaderManager::compileShaders()
{
    for (auto& [name, info] : m_shadersInfos)
    {
        for (auto& shaderInfo : info.shaders)
        {
            if (!compileShader(shaderInfo))
            {
                std::string stageStr = stageToString(shaderInfo.stage);
                RP_LOG(ShaderLog, Error, "Failed to compile shader module for pipeline: {}, stage: {}", name,stageStr);
                return false;
            }
            if (std::filesystem::exists(shaderInfo.path)) continue;
        }
    }
    return true;
}

bool RPE::ShaderManager::compileShader(ShaderModuleInfo& shaderInfo)
{   

    if (!std::filesystem::exists(shaderInfo.sourcePath))
    {
        RP_LOG(ShaderLog, Error, "Shader source file not found: {}", shaderInfo.sourcePath);
        return false;
    }
       
    if (std::filesystem::exists(shaderInfo.path))
    {
        auto sourceTime = std::filesystem::last_write_time(shaderInfo.sourcePath);
        auto spvTime = std::filesystem::last_write_time(shaderInfo.path);
        if (spvTime >= sourceTime)
        {
            RP_LOG(ShaderLog, Display, "Shader already compiled and up to date: {}", shaderInfo.path);
            return true;
        }
    }

  std::string stageStr = stageToString(shaderInfo.stage);
    std::string stageFlag;
    switch (shaderInfo.stage)
    {
        case ShaderStage::Vertex: stageFlag = "-V"; break;
        case ShaderStage::Fragment: stageFlag = "-V"; break;
        case ShaderStage::Geometry: stageFlag = "-V"; break;
        case ShaderStage::Compute: stageFlag = "-V"; break;
        default: RP_LOG(ShaderLog, Error, "Unsupported shader stage for compilation: {}", stageStr); return false;
    }
       
    std::string cmd = "glslangValidator " + stageFlag + " \"" + shaderInfo.sourcePath + "\" -o \"" + shaderInfo.path + "\"";

    RP_LOG(ShaderLog, Display, "Compiling shader: {}", shaderInfo.sourcePath);
    RP_LOG(ShaderLog, Display, "Command: {}", cmd);

   
    int result = std::system(cmd.c_str());
    if (result != 0)
    {
        RP_LOG(ShaderLog, Error, "Failed to compile shader: {}, error code: {}", shaderInfo.sourcePath, result);
        return false;
    }
        
    if (!std::filesystem::exists(shaderInfo.path))
    {
        RP_LOG(ShaderLog, Error, "Compilation completed but SPIR-V file not found: {}", shaderInfo.path);
        return false;
    }

    RP_LOG(ShaderLog, Display, "Shader compiled successfully: {}", shaderInfo.path);
    return true;
}

std::string RPE::ShaderManager::stageToString(const ShaderStage& stage) const
{
    std::string stageStr = "unknown";
    switch (stage)
    {
        case ShaderStage::Vertex: stageStr = "Vertex stage"; break;
        case ShaderStage::Fragment: stageStr = "Fragment stage"; break;
        case ShaderStage::Compute: stageStr = "Compute stage"; break;
        case ShaderStage::TessellationControl: stageStr = "Tessellation control stage"; break;
        case ShaderStage::TessellationEvaluation: stageStr = "Tessellation Evaluation stage"; break;
        case ShaderStage::Geometry: stageStr = "Geometry stage"; break;
        default: stageStr = "unknown"; break;
    }
    return stageStr;
}
