# general info
cmake_minimum_required(VERSION 3.31.6)

#cpp options
set(CMAKE_CXX_STANDARD 20)


# common options
project(RedPorsheEngine VERSION 1.0.0)
add_executable(${PROJECT_NAME} Source/Main.cpp)

#version
configure_file(EngineConfig.h.template ${CMAKE_CURRENT_SOURCE_DIR}/Source/EngineConfig.h)

set(ENGINE_SOURCES
Source/Core/Engine.cpp
Source/Core/Engine.h
.clang-format
Source/pch.cpp
Source/pch.h
Source/EngineConfig.h
)

add_library(${PROJECT_NAME}lib  ${ENGINE_SOURCES})
target_include_directories(${PROJECT_NAME}lib 
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_link_libraries(${PROJECT_NAME}  ${PROJECT_NAME}lib)

#FOLders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME}lib PROPERTIES FOLDER "Engine")
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Engine")

set(CMAKE_SUPPRESS_REGENERATION TRUE)



include(${CMAKE_SOURCE_DIR}/Automation/CMAKE/CMakeHelpers.cmake)

create_ide_folders(ENGINE_SOURCES)
setup_precompiled_headers(${PROJECT_NAME}lib Source/pch.cpp Source/pch.h "${ENGINE_SOURCES}")