#include "InstanceManager.h"
#include "Log/Log.h"
#include "EngineConfig.h"
#include "Core/Engine.h"

#ifdef RPE_USE_NATIVE_WINDOW
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
#endif

#include <vulkan/vulkan.h>

#ifdef RPE_USE_NATIVE_WINDOW
#ifdef _WIN32
#include <vulkan/vulkan_win32.h>
#elif defined(__linux__)
#include <vulkan/vulkan_xcb.h>
#elif defined(__APPLE__)
#include <vulkan/vulkan_metal.h>
#endif
#else
#include <GLFW/glfw3.h>
#endif

using namespace RPE;
DEFINE_LOG_CATEGORY_STATIC(InstanceVkManager);

InstanceManager::InstanceManager()
{
    RP_LOG(InstanceVkManager, Display, "{} created", getName());
}

InstanceManager::~InstanceManager()
{
    shutdown();
}

bool InstanceManager::preInit(const WindowSettings& settings, const std::string& EngineName)
{
    return createInstance(settings, EngineName);
}

bool InstanceManager::init()
{
    if (!createSurface())
    {
        RP_LOG(InstanceVkManager, Error, " Surface not created");
        return false;
    }
    m_isInitialized = true;
    return true;
}

void InstanceManager::shutdown()
{
    if (isInitialized())
    {
        if (surface != VK_NULL_HANDLE)
        {
            if (instance != VK_NULL_HANDLE)
            {
                vkDestroySurfaceKHR(instance, surface, nullptr);
                surface = VK_NULL_HANDLE;
                RP_LOG(InstanceVkManager, Display, "vulkan surface destroyed");
            }
            else
            {
                surface = VK_NULL_HANDLE;
            }
        }
        if (instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(instance, nullptr);
            instance = VK_NULL_HANDLE;
            RP_LOG(InstanceVkManager, Display, "vulkan instance destroyed");
        }
        m_isInitialized = false;
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

VkSurfaceKHR InstanceManager::getSurface() const
{
    return surface;
}

bool InstanceManager::createInstance(const WindowSettings& settings, const std::string& EngineName)
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

    std::vector<const char*> extensions;

#ifdef RPE_USE_NATIVE_WINDOW
    // Нативные расширения для Win32/Linux/macOS
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(__linux__)
    extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(__APPLE__)
    extensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#endif
#else
    // GLFW расширения
    uint32_t glfwExtCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);
    for (uint32_t i = 0; i < glfwExtCount; i++)
        extensions.push_back(glfwExtensions[i]);
#endif

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

bool InstanceManager::createSurface()
{
    if (surface != VK_NULL_HANDLE)
    {
        RP_LOG(InstanceVkManager, Warning, "Surface already created");
        return true;
    }
    auto& engine = Engine::Get();
    const auto window = engine.getMainWindow();

    if (!window || !window->isValid())
    {
        RP_LOG(InstanceVkManager, Error, "Invalid window for surface creation");
        return false;
    }

    void* nativeHandle = window->getWindowHandle();
    if (!nativeHandle)
    {
        RP_LOG(InstanceVkManager, Error, "Failed to get native window handle");
        return false;
    }
    VkResult result{};
    std::string WindowType{};
#ifdef RPE_USE_NATIVE_WINDOW
#ifdef _WIN32
    const auto Win32HWND = static_cast<HWND>(nativeHandle);
    WindowType = "WIN32";
    VkWin32SurfaceCreateInfoKHR surfaceInfo{};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.pNext = nullptr;
    surfaceInfo.hinstance = GetModuleHandle(nullptr);
    surfaceInfo.hwnd = Win32HWND;

    result = vkCreateWin32SurfaceKHR(instance, &surfaceInfo, nullptr, &surface);
#elif defined(__linux__)
    WindowType = "Linux (XCB)";
    // TODO: Implement XCB surface creation
    RP_LOG(InstanceVkManager, Warning, "Linux surface creation not yet implemented");
    result = VK_ERROR_INITIALIZATION_FAILED;
#elif defined(__APPLE__)
    WindowType = "Apple not implemented";
    result = VK_NOT_READY;
#endif
#else
    WindowType = "GLFW";
    const auto WindowHandle = static_cast<GLFWwindow*>(nativeHandle);
    result = glfwCreateWindowSurface(instance, WindowHandle, nullptr, &surface);
#endif

    if (result != VK_SUCCESS)
    {
        RP_LOG(InstanceVkManager, Error, "Fail to create {} surface. Error code: {}", WindowType, static_cast<int>(result));
        return false;
    }
    RP_LOG(InstanceVkManager, Display, "Surface created with window type: {}", WindowType);
    return true;
}
