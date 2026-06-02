# general info
cmake_minimum_required(VERSION 3.31.6)

#cpp options
set(CMAKE_CXX_STANDARD 23)


# common options
project(KuzbassEngine VERSION 1.0.0)
add_executable(${PROJECT_NAME} Source/main.cpp)

#version
configure_file(EngineConfig.h.template ${CMAKE_CURRENT_SOURCE_DIR}/Source/EngineConfig.h)

set(ENGINE_SOURCES
# Engine
Source/Core/Engine.cpp
Source/Core/Engine.h
# utility_source
Source/Core/Utility.h
.clang-format
Source/pch.cpp
Source/pch.h
Source/EngineConfig.h
# logger
Source/Log/Log.h
Source/Log/Log.cpp 
# windows and managers
# GLFW 
Source/Window/GLFW/GLFWWindowManager.h 
Source/Window/GLFW/GLFWWindowManager.cpp 
Source/Window/GLFW/GLFWWindow.h 
Source/Window/GLFW/GLFWWindow.cpp  
 # winapi
Source/Window/WinAPI/Win32WindowManager.h 
Source/Window/WinAPI/Win32WindowManager.cpp 
Source/Window/WinAPI/Win32Window.h 
Source/Window/WinAPI/Win32Window.cpp  
# linux window manager
Source/Window/Linux/LinuxWindowManager.h
Source/Window/Linux/LinuxWindowManager.cpp
Source/Window/Linux/LinuxWindow.h
Source/Window/Linux/LinuxWindow.cpp
# macos window manager
Source/Window/MacOS/MacOSWindowManager.h    
Source/Window/MacOS/MacOSWindowManager.cpp
Source/Window/MacOS/MacOSWindow.h
Source/Window/MacOS/MacOSWindow.cpp
# interfaces
Source/Window/IWindowManager.h 
Source/Window/IWindow.h 
# events
Source/Event/Event.h 
Source/Event/InputEvent.h
# input manager
Source/Input/InputManager.h
Source/Input/InputManager.cpp 
Source/Input/KeyCodes.h
Source/GameFrameWork/Controller/Controller.h
Source/GameFrameWork/Controller/Controller.cpp
Source/GameFrameWork/Controller/PlayerController.h
Source/GameFrameWork/Controller/PlayerController.cpp
# World 
Source/GameFrameWork/GameObjects/WorldObject/WorldObject.h 
Source/GameFrameWork/GameObjects/WorldObject/WorldObject.cpp
Source/GameFrameWork/GameObjects/Components/InputComponent.h 
Source/GameFrameWork/GameObjects/Components/InputComponent.cpp
)

add_library(${PROJECT_NAME}lib  ${ENGINE_SOURCES})
target_include_directories(${PROJECT_NAME}lib 
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_link_libraries(${PROJECT_NAME}  ${PROJECT_NAME}lib)

if(MSVC)
target_compile_options(${PROJECT_NAME}lib PUBLIC /Zc:preprocessor)
endif()

# tests 
set(TEST_SOURCES
Tests/LogConceptTests.cpp 
Tests/WindowTests.cpp 
Tests/EventTests.cpp 
Tests/EventChaining.cpp
Tests/EventSubscriptionLifetime.cpp
)
add_executable(${PROJECT_NAME}Tests ${TEST_SOURCES})


# add libs
find_package(spdlog REQUIRED)
find_package(glfw3)
target_link_libraries(${PROJECT_NAME}lib PUBLIC spdlog::spdlog glfw)

find_package(GTest REQUIRED)
target_link_libraries(${PROJECT_NAME}Tests ${PROJECT_NAME}lib gtest::gtest glfw)

#FOLders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME}lib PROPERTIES FOLDER "Engine")
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Engine")

set(CMAKE_SUPPRESS_REGENERATION TRUE)



include(${CMAKE_SOURCE_DIR}/Automation/CMAKE/CMakeHelpers.cmake)

create_ide_folders(ENGINE_SOURCES)
setup_precompiled_headers(${PROJECT_NAME}lib Source/pch.cpp Source/pch.h "${ENGINE_SOURCES}")