#include "Core/Engine.h"
#include <cstdlib>
#include "Window/GLFW/GLFWWindowManager.h"
#include "Window/WinAPI/Win32WindowManager.h"

int main()
{
    auto WindowManager = std::make_unique<RPE::GLFWWindowManager>();
    // auto WindowManager = std::make_unique<RPE::Win32WindowManager>();
    RPE::Engine engine{std::move(WindowManager)};
    engine.run();

    return EXIT_SUCCESS;
}