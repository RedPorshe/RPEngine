#include "DeviceManager.h"
#include "..\vkContext.h"
#include "InstanceManager.h"
#include "Log/Log.h"
#include <vector>

DEFINE_LOG_CATEGORY_STATIC(LogDeviceManager);
using namespace RPE;

DeviceManager::DeviceManager() {}

DeviceManager::~DeviceManager() {}

bool DeviceManager::preInit(const WindowSettings& settings, const std::string& EngineName)
{
    if (m_contextPtr != nullptr)
    {
        RP_LOG(LogDeviceManager, Display, "{} has vulkan context ptr", getName());
        return true;
    }
    return false;
}

bool DeviceManager::init()
{
    RP_LOG(LogDeviceManager, Display, "Initializing {}...", getName());
    bool success = pickPhysicalDevice();
    if (!success)
    {
        RP_LOG(LogDeviceManager, Error, "{} failed to pick physical device", getName());
        return false;
    }
    success = createDevice();
    if (!success)
    {
        RP_LOG(LogDeviceManager, Error, "{} failed to create device", getName());
        return false;
    }

    m_isInitialized = success;
    return success;
}

void DeviceManager::shutdown()
{
    if (isInitialized())
    {
        RP_LOG(LogDeviceManager, Display, "Shutting down {}...", getName());

        if (m_device != VK_NULL_HANDLE)
        {
            vkDeviceWaitIdle(m_device);
            vkDestroyDevice(m_device, nullptr);
            m_device = VK_NULL_HANDLE;
            RP_LOG(LogDeviceManager, Display, "Logical device destroyed");
        }

        m_physicalDevice = VK_NULL_HANDLE;
        m_isInitialized = false;
    }
}

std::string DeviceManager::getName()
{
    return "[Device Manager]";
}

VkDevice DeviceManager::getDevice() const
{
    return m_device;
}

VkPhysicalDevice DeviceManager::getPhysicalDevice() const
{
    return m_physicalDevice;
}

int RPE::DeviceManager::getGraphicsQueueIndex() const
{
    return m_graphicsQueueIndex;
}

int RPE::DeviceManager::getPresentQueueIndex() const
{
    return m_presentQueueIndex;
}

bool DeviceManager::pickPhysicalDevice()
{
    auto instanceMgr = m_contextPtr->getInstanceManager();
    if (!instanceMgr)
    {
        RP_LOG(LogDeviceManager, Error, "{} failed  to pick physical device instance manager is NULL_HANDLE", getName());
        return false;
    }
    auto instance = instanceMgr->getInstance();
    if (instance == VK_NULL_HANDLE)
    {
        RP_LOG(LogDeviceManager, Error, "{} failed  to pick physical device instance is NULL_HANDLE", getName());
        return false;
    }
    uint32_t deviceCount{0};
    VkResult result{};
    result = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (result != VK_SUCCESS)
    {
        RP_LOG(LogDeviceManager, Error, "{} failed to enumerate physical devices: {}", getName(), static_cast<int>(result));
        return false;
    }
    if (deviceCount == 0)
    {
        RP_LOG(LogDeviceManager, Error, "{} failed: No physical devices found", getName());
        return false;
    }

    std::vector<VkPhysicalDevice> physDevices(deviceCount);

    result = vkEnumeratePhysicalDevices(instance, &deviceCount, physDevices.data());
    if (result != VK_SUCCESS)
    {
        RP_LOG(LogDeviceManager, Error, "{} failed to get physical devices list: {}", getName(), static_cast<int>(result));
        return false;
    }

    m_physicalDevice = VK_NULL_HANDLE;

    for (auto device : physDevices)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        if (queueFamilyCount > 0)
        {
            m_physicalDevice = device;
            RP_LOG(LogDeviceManager, Display, "{} selected physical device: {}", getName(), properties.deviceName);
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE)
    {
        RP_LOG(LogDeviceManager, Error, "{} failed: No suitable physical device found", getName());
        return false;
    }

    return true;
}

bool DeviceManager::createDevice()
{
    if (m_physicalDevice == VK_NULL_HANDLE)
    {
        RP_LOG(LogDeviceManager, Error, "{} failed: Physical device is null", getName());
        return false;
    }

    auto instanceMgr = m_contextPtr->getInstanceManager();
    if (!instanceMgr)
    {
        RP_LOG(LogDeviceManager, Error, "{} failed: InstanceManager is null", getName());
        return false;
    }

    VkSurfaceKHR surface = instanceMgr->getSurface();
    if (surface == VK_NULL_HANDLE)
    {
        RP_LOG(LogDeviceManager, Error, "{} failed: Surface is null", getName());
        return false;
    }

    // Находим очереди
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, queueFamilies.data());

    int graphicsQueueIndex = -1;
    int presentQueueIndex = -1;

    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsQueueIndex = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, i, surface, &presentSupport);
        if (presentSupport)
        {
            presentQueueIndex = i;
        }

        if (graphicsQueueIndex != -1 && presentQueueIndex != -1)
        {
            break;
        }
    }

    if (graphicsQueueIndex == -1)
    {
        RP_LOG(LogDeviceManager, Error, "{} failed: No graphics queue family found", getName());
        return false;
    }

    // Создаём очереди
    float queuePriority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    VkDeviceQueueCreateInfo graphicsQueueCreateInfo{};
    graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphicsQueueCreateInfo.queueFamilyIndex = static_cast<uint32_t>(graphicsQueueIndex);
    graphicsQueueCreateInfo.queueCount = 1;
    graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(graphicsQueueCreateInfo);

    if (presentQueueIndex != graphicsQueueIndex && presentQueueIndex != -1)
    {
        VkDeviceQueueCreateInfo presentQueueCreateInfo{};
        presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        presentQueueCreateInfo.queueFamilyIndex = static_cast<uint32_t>(presentQueueIndex);
        presentQueueCreateInfo.queueCount = 1;
        presentQueueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(presentQueueCreateInfo);
    }

    // Расширения устройства
    std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    // Валидационные слои
    std::vector<const char*> validationLayers;
#ifdef _DEBUG
    validationLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    // Получаем поддерживаемые фичи устройства
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(m_physicalDevice, &supportedFeatures);

    // Настраиваем фичи (включаем только поддерживаемые)
    VkPhysicalDeviceFeatures deviceFeatures{};

    // Основные фичи
    if (supportedFeatures.robustBufferAccess)
    {
        deviceFeatures.robustBufferAccess = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Robust buffer access: enabled");
    }

    if (supportedFeatures.fullDrawIndexUint32)
    {
        deviceFeatures.fullDrawIndexUint32 = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Full draw index uint32: enabled");
    }

    if (supportedFeatures.imageCubeArray)
    {
        deviceFeatures.imageCubeArray = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Image cube array: enabled");
    }

    if (supportedFeatures.independentBlend)
    {
        deviceFeatures.independentBlend = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Independent blend: enabled");
    }

    if (supportedFeatures.geometryShader)
    {
        deviceFeatures.geometryShader = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Geometry shader: enabled");
    }

    if (supportedFeatures.tessellationShader)
    {
        deviceFeatures.tessellationShader = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Tessellation shader: enabled");
    }

    if (supportedFeatures.sampleRateShading)
    {
        deviceFeatures.sampleRateShading = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Sample rate shading: enabled");
    }

    if (supportedFeatures.dualSrcBlend)
    {
        deviceFeatures.dualSrcBlend = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Dual source blend: enabled");
    }

    if (supportedFeatures.logicOp)
    {
        deviceFeatures.logicOp = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Logic op: enabled");
    }

    if (supportedFeatures.multiDrawIndirect)
    {
        deviceFeatures.multiDrawIndirect = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Multi draw indirect: enabled");
    }

    if (supportedFeatures.drawIndirectFirstInstance)
    {
        deviceFeatures.drawIndirectFirstInstance = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Draw indirect first instance: enabled");
    }

    if (supportedFeatures.depthClamp)
    {
        deviceFeatures.depthClamp = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Depth clamp: enabled");
    }

    if (supportedFeatures.depthBiasClamp)
    {
        deviceFeatures.depthBiasClamp = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Depth bias clamp: enabled");
    }

    if (supportedFeatures.fillModeNonSolid)
    {
        deviceFeatures.fillModeNonSolid = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Fill mode non-solid (wireframe): enabled");
    }

    if (supportedFeatures.depthBounds)
    {
        deviceFeatures.depthBounds = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Depth bounds: enabled");
    }

    if (supportedFeatures.wideLines)
    {
        deviceFeatures.wideLines = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Wide lines: enabled");
    }

    if (supportedFeatures.largePoints)
    {
        deviceFeatures.largePoints = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Large points: enabled");
    }

    if (supportedFeatures.alphaToOne)
    {
        deviceFeatures.alphaToOne = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Alpha to one: enabled");
    }

    if (supportedFeatures.multiViewport)
    {
        deviceFeatures.multiViewport = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Multi viewport: enabled");
    }

    if (supportedFeatures.samplerAnisotropy)
    {
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Sampler anisotropy: enabled");
    }

    if (supportedFeatures.textureCompressionETC2)
    {
        deviceFeatures.textureCompressionETC2 = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Texture compression ETC2: enabled");
    }

    if (supportedFeatures.textureCompressionASTC_LDR)
    {
        deviceFeatures.textureCompressionASTC_LDR = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Texture compression ASTC LDR: enabled");
    }

    if (supportedFeatures.textureCompressionBC)
    {
        deviceFeatures.textureCompressionBC = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Texture compression BC: enabled");
    }

    if (supportedFeatures.occlusionQueryPrecise)
    {
        deviceFeatures.occlusionQueryPrecise = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Occlusion query precise: enabled");
    }

    if (supportedFeatures.pipelineStatisticsQuery)
    {
        deviceFeatures.pipelineStatisticsQuery = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Pipeline statistics query: enabled");
    }

    if (supportedFeatures.vertexPipelineStoresAndAtomics)
    {
        deviceFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Vertex pipeline stores and atomics: enabled");
    }

    if (supportedFeatures.fragmentStoresAndAtomics)
    {
        deviceFeatures.fragmentStoresAndAtomics = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Fragment stores and atomics: enabled");
    }

    if (supportedFeatures.shaderTessellationAndGeometryPointSize)
    {
        deviceFeatures.shaderTessellationAndGeometryPointSize = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Shader tessellation/geometry point size: enabled");
    }

    if (supportedFeatures.shaderImageGatherExtended)
    {
        deviceFeatures.shaderImageGatherExtended = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Shader image gather extended: enabled");
    }

    if (supportedFeatures.shaderStorageImageExtendedFormats)
    {
        deviceFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Shader storage image extended formats: enabled");
    }

    if (supportedFeatures.shaderStorageImageMultisample)
    {
        deviceFeatures.shaderStorageImageMultisample = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Shader storage image multisample: enabled");
    }

    if (supportedFeatures.shaderStorageImageReadWithoutFormat)
    {
        deviceFeatures.shaderStorageImageReadWithoutFormat = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Shader storage image read without format: enabled");
    }

    if (supportedFeatures.shaderStorageImageWriteWithoutFormat)
    {
        deviceFeatures.shaderStorageImageWriteWithoutFormat = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Shader storage image write without format: enabled");
    }

    if (supportedFeatures.shaderUniformBufferArrayDynamicIndexing)
    {
        deviceFeatures.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Shader uniform buffer array dynamic indexing: enabled");
    }

    if (supportedFeatures.shaderSampledImageArrayDynamicIndexing)
    {
        deviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Shader sampled image array dynamic indexing: enabled");
    }

    if (supportedFeatures.shaderStorageBufferArrayDynamicIndexing)
    {
        deviceFeatures.shaderStorageBufferArrayDynamicIndexing = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Shader storage buffer array dynamic indexing: enabled");
    }

    if (supportedFeatures.shaderStorageImageArrayDynamicIndexing)
    {
        deviceFeatures.shaderStorageImageArrayDynamicIndexing = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Shader storage image array dynamic indexing: enabled");
    }

    if (supportedFeatures.shaderClipDistance)
    {
        deviceFeatures.shaderClipDistance = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Shader clip distance: enabled");
    }

    if (supportedFeatures.shaderCullDistance)
    {
        deviceFeatures.shaderCullDistance = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Shader cull distance: enabled");
    }

    if (supportedFeatures.shaderFloat64)
    {
        deviceFeatures.shaderFloat64 = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Shader float64: enabled");
    }

    if (supportedFeatures.shaderInt64)
    {
        deviceFeatures.shaderInt64 = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Shader int64: enabled");
    }

    if (supportedFeatures.shaderInt16)
    {
        deviceFeatures.shaderInt16 = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Shader int16: enabled");
    }

    if (supportedFeatures.inheritedQueries)
    {
        deviceFeatures.inheritedQueries = VK_TRUE;
        RP_LOG(LogDeviceManager, Display, "  Inherited queries: enabled");
    }

    // Создаём устройство
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    VkResult result = vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device);
    if (result != VK_SUCCESS)
    {
        RP_LOG(LogDeviceManager, Error, "{} failed to create logical device: {}", getName(), static_cast<int>(result));
        return false;
    }

    m_graphicsQueueIndex = graphicsQueueIndex;
    m_presentQueueIndex = presentQueueIndex;

    RP_LOG(LogDeviceManager, Display, "{} created logical device successfully", getName());
    RP_LOG(LogDeviceManager, Display, "  Graphics queue index: {}, Present queue index: {}", graphicsQueueIndex, presentQueueIndex);

    return true;
}