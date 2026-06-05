#include "Core/Engine.h"
#include "Log/Log.h"
#include <cstdlib>
DEFINE_LOG_CATEGORY_STATIC(MainLog);

#ifdef RPE_USE_NATIVE_WINDOW
#ifdef _WIN32
#include "Window/WinAPI/Win32WindowManager.h"
#elif defined(__linux__)
#include "Window/Linux/LinuxWindowManager.h"
#elif defined(__APPLE__)
#include "Window/MacOS/MacOSWindowManager.h"
#endif  // WIN32
#else
#include "Window/GLFW/GLFWWindowManager.h"
#endif  // RPE_USE_NATIVE_WINDOW

#ifdef RPE_USE_VULKAN
#include "Render/vulkan/vkRender.h"
#else
#include "Render/vulkan/vkRender.h"  //stub #include "Render/DX3D/dxRender.h"
#endif

void logInitializationError(const std::string& managerName)
{
    RP_LOG(MainLog, Error, "Failed to initialize the {} window manager.", managerName);
}

void logInitializationSuccess(const std::string& managerName)
{
    RP_LOG(MainLog, Display, "Successfully initialized the {} window manager.", managerName);
}


template <typename WindowManagerT>
void testWindowManager(const std::string& name)
{
    auto windowManager = std::make_unique<WindowManagerT>();
    if (!windowManager->Isinitialized())
        logInitializationError(name);
    else
        logInitializationSuccess(name);
}


void TestWindowManagerInitialization()
{
#ifdef RPE_USE_NATIVE_WINDOW
    RP_LOG(MainLog, Display, "Testing native window manager initialization...");
#ifdef _WIN32
    {
        RP_LOG(MainLog, Display, "Testing Win32 window manager initialization...");
        testWindowManager<RPE::Win32WindowManager>("Win32");       
    }
#elif defined(__linux__)

    {
        RP_LOG(MainLog, Display, "Testing Linux window manager initialization...");
        testWindowManager<RPE::LinuxWindowManager>("Linux");       
    }
#elif defined(__APPLE__)

    {
        RP_LOG(MainLog, Display, "Testing macOS window manager initialization...");
        testWindowManager<RPE::MacOSWindowManager>("macOS");        
    }
#endif
#else
    RP_LOG(MainLog, Display, "Testing GLFW window manager initialization...");
    {
        testWindowManager<RPE::GLFWWindowManager>("GLFW");       
    }
#endif
}

int main()
{
#ifdef SKIP_WINDOW_TESTS
    RP_LOG(MainLog, Display, "==============Starting Window managers tests======");
    TestWindowManagerInitialization();
    RP_LOG(MainLog, Display, "===========Finished Window managers tests==========");
#endif  // !SKIP_WINDOW_TESTS

    std::unique_ptr<RPE::IWindowManager> WindowManager;

#ifdef RPE_USE_NATIVE_WINDOW
#ifdef _WIN32
    WindowManager = std::make_unique<RPE::Win32WindowManager>();

#elif defined(__linux__)
    WindowManager = std::make_unique<RPE::LinuxWindowManager>();

#elif defined(__APPLE__)
    WindowManager = std::make_unique<RPE::MacOSWindowManager>();
#endif  // WIN32
#else
    WindowManager = std::make_unique<RPE::GLFWWindowManager>();

#endif  // RPE_USE_NATIVE_WINDOW
    std::unique_ptr<RPE::RHI> Renderer;

#ifdef RPE_USE_VULKAN
    Renderer = std::make_unique<RPE::VkRenderer>();
#else
    Renderer = std::make_unique<RPE::VkRenderer>();
    // stub
#endif

    if (!WindowManager)
    {
        RP_LOG(MainLog, Error, "Failed to create the window manager.");
        return EXIT_FAILURE;
    }
    if (!Renderer)
    {
        RP_LOG(MainLog, Error, "Failed to create Renderer...");
        return EXIT_FAILURE;
    }

    RPE::Engine engine{std::move(WindowManager), std::move(Renderer)};
    if (!engine.isInitialized())
    {
        RP_LOG(MainLog, Error, "Failed to initialize the engine.");
        return EXIT_FAILURE;
    }
    else
    {
        engine.run();
    }

    return EXIT_SUCCESS;
}