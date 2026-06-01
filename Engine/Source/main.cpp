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

void logInitializationError(const std::string& managerName)
{
    RP_LOG(MainLog, Error, "Failed to initialize the {} window manager.", managerName);
}

void logInitializationSuccess(const std::string& managerName)
{
    RP_LOG(MainLog, Display, "Successfully initialized the {} window manager.", managerName);
}

void TestWindowManagerInitialization()
{

#ifdef RPE_USE_NATIVE_WINDOW
    RP_LOG(MainLog, Display, "Testing native window manager initialization...");
#ifdef _WIN32
    {
        RP_LOG(MainLog, Display, "Testing Win32 window manager initialization...");
        auto windowManager = std::make_unique<RPE::Win32WindowManager>();
        if (!windowManager->Isinitialized())
        {
            logInitializationError("Win32");
        }
        else
        {
            logInitializationSuccess("Win32");
        }
        windowManager.reset();
    }
#elif defined(__linux__)

    {
        RP_LOG(MainLog, Display, "Testing Linux window manager initialization...");
        auto windowManager = std::make_unique<RPE::LinuxWindowManager>();
        if (!windowManager->Isinitialized())
        {
            logInitializationError("Linux");
        }
        else
        {
            logInitializationSuccess("Linux");
        }
        windowManager.reset();
    }
#elif defined(__APPLE__)

    {
        RP_LOG(MainLog, Display, "Testing macOS window manager initialization...");)
        auto windowManager = std::make_unique<RPE::MacOSWindowManager>();
        if (!windowManager->Isinitialized())
        {
            logInitializationError("macOS");
        }
        else
        {
            logInitializationSuccess("macOS");
        }
        windowManager.reset();
    }
#endif
#else
    RP_LOG(MainLog, Display, "Testing GLFW window manager initialization...");
    {
        auto windowManager = std::make_unique<RPE::GLFWWindowManager>();
        if (!windowManager->Isinitialized())
        {
            logInitializationError("GLFW");
        }
        else
        {
            logInitializationSuccess("GLFW");
        }
        windowManager.reset();
    }
#endif
}

int main()
{
    RP_LOG(MainLog, Display, "Starting Window managers tests");
    TestWindowManagerInitialization();
    RP_LOG(MainLog, Display, "Finished Window managers tests");

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

    if (!WindowManager)
    {
        RP_LOG(MainLog, Error, "Failed to create the window manager.");
        return EXIT_FAILURE;
    }

    RPE::Engine engine{std::move(WindowManager)};
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