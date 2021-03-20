cmake_minimum_required(VERSION 3.17)
project(rfx)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(USE_PRECOMPILED_HEADERS ON)

file(GLOB_RECURSE BASE_SOURCE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

file(GLOB_RECURSE BASE_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

include_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/test"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/ktx/include"
        "$ENV{VULKAN_SDK}/include"
)

link_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/ktx/lib"
        $ENV{VULKAN_SDK}/lib)

link_libraries(
        vulkan-1
        ktx)

find_package(glfw3 CONFIG REQUIRED)
link_libraries(glfw)

find_package(glm CONFIG REQUIRED)
link_libraries(glm)

find_package(glslang CONFIG REQUIRED)
link_libraries(glslang::HLSL glslang::SPIRV glslang::glslang glslang::OGLCompiler)

find_path(STB_INCLUDE_DIRS  "stb.h")
include_directories(${STB_INCLUDE_DIRS})

find_package(imgui CONFIG REQUIRED)
link_libraries(imgui::imgui)

find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
include_directories(${TINYGLTF_INCLUDE_DIRS})

find_package(nlohmann_json CONFIG REQUIRED)
link_libraries(nlohmann_json nlohmann_json::nlohmann_json)

find_package(fmt CONFIG REQUIRED)
link_libraries(fmt::fmt)

add_subdirectory(test)