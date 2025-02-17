cmake_minimum_required(VERSION 3.8)
project(ProceduralTool VERSION 1.0 LANGUAGES CXX)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "Custom" FORCE)
message("Generated with config types: ${CMAKE_CONFIGURATION_TYPES}")

# Find packages
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# Library source files
file(GLOB_RECURSE PROCEDURAL_TOOL_SOURCES 
    "${CMAKE_SOURCE_DIR}/Library/*.cpp"
    "${CMAKE_SOURCE_DIR}/Library/*.h"
)

# Static Library
add_library(Library STATIC ${PROCEDURAL_TOOL_SOURCES})

target_include_directories(Library PRIVATE 
    "${CMAKE_SOURCE_DIR}/Library"
)

target_link_libraries(Library PRIVATE 
    glm::glm
    glad::glad
    nlohmann_json::nlohmann_json
    imgui::imgui
    glfw
)

# Client executable source files
file(GLOB_RECURSE CLIENT_SOURCES 
    "${CMAKE_SOURCE_DIR}/Client/*.cpp"
    "${CMAKE_SOURCE_DIR}/Client/*.h"
)

# Debug output
message("Client sources: ${CLIENT_SOURCES}")

# Verify Client sources exist
if(NOT CLIENT_SOURCES)
    message(FATAL_ERROR "No source files found for Client in ${CMAKE_SOURCE_DIR}/Client/")
endif()

# Client executable
add_executable(Client ${CLIENT_SOURCES})

target_include_directories(Client PRIVATE 
    "${CMAKE_SOURCE_DIR}/Client"
    "${CMAKE_SOURCE_DIR}/Library"
)

target_link_libraries(Client PRIVATE 
    Library
    glm::glm
)

# Disable debug symbols for RelWithDebInfo build
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -DNDEBUG")