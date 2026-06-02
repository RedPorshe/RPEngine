#include "vkRender.h"
#include "vkContext.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(VkRenderLog);

using namespace RPE;

VkRenderer::VkRenderer() : m_ctx(std::make_unique<VulkanContext>())
{
    RP_LOG(VkRenderLog, Display, "{} created with engine version {}", getName(), getVersion());
}

VkRenderer::~VkRenderer()
{
    shutdown();
}

bool VkRenderer::preInit(WindowSettings settings, const std::string& EngineName)
{
    if (!m_ctx->preInit(settings, EngineName)) return false;
    return true;  // now stub
}

bool VkRenderer::init(IWindow* windowHandle)
{
    m_WindowHandle = windowHandle->getWindowHandle();
    if (m_WindowHandle == nullptr) return false;
    if (!m_ctx->init(windowHandle))
    {
        RP_LOG(VkRenderLog, Error, "{} init failed", getName());
        return false;
    }
    RP_LOG(VkRenderLog, Display, "{} init success", getName());
    return true;
}

void VkRenderer::update() {}

void VkRenderer::setEnginePtr(Engine* ptr)
{
    m_EnginePtr = ptr;
    if (m_EnginePtr)
    {
        RP_LOG(VkRenderLog, Display, "Engine ptr succes gained");
    }
    else
    {
        RP_LOG(VkRenderLog, Display, "Engine ptr succes cleared");
    }
}

bool VkRenderer::render()
{

    return true;  // stub
}

void VkRenderer::shutdown()
{
    RP_LOG(VkRenderLog, Display, " {} starting shutdown", getName());
}

std::string VkRenderer::getName() const
{
    return "[Vulkan Renderer]";
}

std::string VkRenderer::getVersion() const
{
    return m_version;
}

void VkRenderer::onResize(int width, int height)
{
    if (m_ctx) m_ctx->OnResize(width, height);
}
