#pragma once
#include "IVkManager.h"
#include <unordered_map>
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace RPE
{
enum class ShaderStage
{
    Vertex = VK_SHADER_STAGE_VERTEX_BIT,                                   // 0x00000001 = 1
    Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,                               // 0x00000010 = 16
    Geometry = VK_SHADER_STAGE_GEOMETRY_BIT,                               // 0x00000008 = 8
    Compute = VK_SHADER_STAGE_COMPUTE_BIT,                                 // 0x00000020 = 32
    TessellationControl = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,        // 0x00000002 = 2
    TessellationEvaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,  // 0x00000004 = 4
};

struct ShaderModuleInfo
{
    ShaderStage stage;
    std::string path;  // Путь к .spv файлу
    std::string sourcePath;
    std::string entryPoint = "main";
    VkShaderModule module{VK_NULL_HANDLE};
};

struct ShadersInfo
{
    std::string pipelineName{};
    std::vector<ShaderModuleInfo> shaders;

    bool hasVertex() const
    {
        return std::any_of(shaders.begin(), shaders.end(), [](const ShaderModuleInfo& s) { return s.stage == ShaderStage::Vertex; });
    }

    bool hasFragment() const
    {
        return std::any_of(shaders.begin(), shaders.end(), [](const ShaderModuleInfo& s) { return s.stage == ShaderStage::Fragment; });
    }

    bool hasCompute() const
    {
        return std::any_of(shaders.begin(), shaders.end(), [](const ShaderModuleInfo& s) { return s.stage == ShaderStage::Compute; });
    }

    VkShaderModule getModule(ShaderStage stage) const
    {
        auto it = std::find_if(shaders.begin(), shaders.end(), [stage](const ShaderModuleInfo& s) { return s.stage == stage; });
        return it != shaders.end() ? it->module : VK_NULL_HANDLE;
    }
};

class ShaderManager : public IVkManager
{
public:
    ShaderManager();
    ~ShaderManager() override;

    bool preInit(const WindowSettings& settings, const std::string& EngineName) override;
    bool init() override;
    void shutdown() override;
    std::string getName() override;

    ShadersInfo* getShaders(const std::string& pipelineName);
    bool hasShaders(const std::string& pipelineName) const;

private:
    bool loadShaderModule(ShaderModuleInfo& shaderInfo);
    bool createShaderModules();
    bool compileShaders();
    bool compileShader(ShaderModuleInfo& shaderInfo);
    std::string stageToString(const ShaderStage& stage) const;

    std::unordered_map<std::string, ShadersInfo> m_shadersInfos;
};
}  // namespace RPE