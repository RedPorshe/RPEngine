# general info
cmake_minimum_required(VERSION 3.31.6)

#cpp options
set(CMAKE_CXX_STANDARD 23)


# common options
project(RedPorsheEngine VERSION 1.0.0)
add_executable(${PROJECT_NAME} Source/main.cpp)

#version
configure_file(EngineConfig.h.template ${CMAKE_CURRENT_SOURCE_DIR}/Source/EngineConfig.h)

set(ENGINE_SOURCES
Source/Core/Engine.cpp
Source/Core/Engine.h
Source/Core/Utility.h
.clang-format
Source/pch.cpp
Source/pch.h
Source/EngineConfig.h
Source/Log/Log.h
Source/Log/Log.cpp 
Source/Window/GLFW/GLFWWindowManager.h 
Source/Window/GLFW/GLFWWindowManager.cpp 
Source/Window/GLFW/GLFWWindow.h 
Source/Window/GLFW/GLFWWindow.cpp  
Source/Window/IWindowManager.h 
Source/Window/IWindowManager.cpp 
Source/Window/IWindow.h 
Source/Window/IWindow.cpp  
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
Tests/LogConceptsTest.cpp 
Tests/LogConceptTests.cpp 
Tests/WindowTests.cpp 
)
add_executable(${PROJECT_NAME}TestRunner ${TEST_SOURCES})


# add libs
find_package(spdlog REQUIRED)
find_package(glfw3)
target_link_libraries(${PROJECT_NAME}lib PUBLIC spdlog::spdlog glfw)

find_package(GTest REQUIRED)
target_link_libraries(${PROJECT_NAME}TestRunner ${PROJECT_NAME}lib gtest::gtest glfw)

#FOLders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME}lib PROPERTIES FOLDER "Engine")
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Engine")

set(CMAKE_SUPPRESS_REGENERATION TRUE)



include(${CMAKE_SOURCE_DIR}/Automation/CMAKE/CMakeHelpers.cmake)

create_ide_folders(ENGINE_SOURCES)
setup_precompiled_headers(${PROJECT_NAME}lib Source/pch.cpp Source/pch.h "${ENGINE_SOURCES}")