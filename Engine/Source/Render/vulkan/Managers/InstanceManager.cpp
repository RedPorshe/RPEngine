#include "InstanceManager.h"
#include "Log/Log.h"
#include "EngineConfig.h"

#include <GLFW/glfw3.h>

using namespace RPE;
DEFINE_LOG_CATEGORY_STATIC(InstanceVkManager);

RPE::InstanceManager::InstanceManager()
{
    RP_LOG(InstanceVkManager, Display, "{} created", getName());
}

RPE::InstanceManager::~InstanceManager()
{
    shutdown();
}

bool InstanceManager::preInit(const WindowSettings& settings, const std::string& EngineName)
{
    return createInstance(settings, EngineName);
}

bool InstanceManager::init()
{
    return true;
}

void InstanceManager::shutdown()
{
    if (instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }
}

std::string InstanceManager::getName()
{
    return "[Instance Manager]";
}

VkInstance InstanceManager::getInstance() const
{
    return instance;
}

bool RPE::InstanceManager::createInstance(const WindowSettings& settings, const std::string& EngineName)
{
    VkApplicationInfo appinfo{};
    appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appinfo.pNext = nullptr;
    appinfo.pEngineName = EngineName.c_str();
    appinfo.engineVersion = VK_MAKE_VERSION(Engine_VERSION_MAJOR, Engine_VERSION_MINOR, Engine_VERSION_PATCH);
    appinfo.pApplicationName = settings.title.c_str();
    appinfo.applicationVersion = VK_MAKE_VERSION(Engine_VERSION_MAJOR, Engine_VERSION_MINOR, Engine_VERSION_PATCH);

#ifdef VK_API_VERSION_1_4
    appinfo.apiVersion = VK_API_VERSION_1_4;
#else
    appinfo.apiVersion = VK_API_VERSION_1_3;
#endif

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    if (!glfwExtensions)
    {
        RP_LOG(InstanceVkManager, Error, "Failed to get GLFW required extensions");
        return false;
    }

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef _DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    RP_LOG(InstanceVkManager, Display, "Added debug utils extension");
#endif

    std::vector<const char*> validationLayers;
#ifdef _DEBUG
    validationLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    VkInstanceCreateInfo creatinfo{};
    creatinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    creatinfo.pNext = nullptr;
    creatinfo.pApplicationInfo = &appinfo;
    creatinfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    creatinfo.ppEnabledExtensionNames = extensions.data();
    creatinfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    creatinfo.ppEnabledLayerNames = validationLayers.data();

    VkResult result = vkCreateInstance(&creatinfo, nullptr, &instance);
    if (result != VK_SUCCESS)
    {
        RP_LOG(InstanceVkManager, Error, "Fail to create instance. Error code: {}", static_cast<int>(result));
        return false;
    }
    RP_LOG(InstanceVkManager, Display, "Vulkan instance created successfully");
    RP_LOG(InstanceVkManager, Display, "  Extensions enabled: {}", extensions.size());
    for (const auto& ext : extensions)
    {
        RP_LOG(InstanceVkManager, Display, "    - {}", ext);
    }
    return true;
}
