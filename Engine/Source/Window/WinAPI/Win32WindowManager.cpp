#include "Win32WindowManager.h"
#include "Win32Window.h"
#include "Log/Log.h"

using namespace RPE;

DEFINE_LOG_CATEGORY_STATIC(LogWin32WindowManager);

Win32WindowManager::Win32WindowManager()
{
    RP_LOG(LogWin32WindowManager, Display, "Win32 API window manager Created!");
  
#ifdef RPE_USE_VULKAN
    
#endif
    m_initialized = true;
}

Win32WindowManager::~Win32WindowManager() 
    {
    m_Windows.clear();
    if (m_initialized)
    {

    }
    m_initialized = false;
    RP_LOG(LogWin32WindowManager, Display, "Win32 Window Manager shutdown complete!");
    }

void Win32WindowManager::update() 
    {
    if (!Isinitialized()) return;

     MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    cleanupClosedWindows();

    }

bool Win32WindowManager::areAllWindowsClosed() const
{
    return m_Windows.empty();
}

std::expected<WindowId, WindowCreationerror> Win32WindowManager::createWindow(const WindowSettings& settings)
{
    if (!Isinitialized())
    {
        RP_LOG(LogWin32WindowManager, Error, "Cannot create window. Win32 is not initialized");
        return std::unexpected(WindowCreationerror::ManagerIsNotInitialized);
    }
    auto window = std::make_shared<Win32Window>(m_windowIdCounter, settings);
    if (!window->isValid())
    {
        RP_LOG(LogWin32WindowManager, Error, "Failed  to create WinAPI window.");
        return std::unexpected(WindowCreationerror::InvalidWindow);
    }
    const WindowId id = m_windowIdCounter++;
    m_Windows[id] = window;

    RP_LOG(LogWin32WindowManager, Display, "Added window with id: {}", id.value);

    return id;
}

std::shared_ptr<IWindow> Win32WindowManager::getWindowById(WindowId id) const
{
    const auto it = m_Windows.find(id);
    return it != m_Windows.end() ? it->second : nullptr;
}

bool RPE::Win32WindowManager::Isinitialized() const
{
    return m_initialized;
}

void Win32WindowManager::cleanupClosedWindows() 
    {
    auto it = m_Windows.begin();
    while (it != m_Windows.end())
    {
        if (it->second->shouldClose())
        {
            RP_LOG(LogWin32WindowManager, Display, "Remove closed window with id: {}", it->first.value);
            it = m_Windows.erase(it);
            continue;
        }
        ++it;
    }
    }
