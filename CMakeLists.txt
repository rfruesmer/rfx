cmake_minimum_required(VERSION 3.17)
project(rfx)

set(CMAKE_CXX_STANDARD 20)

add_executable(rfx
        src/rfx/common/Logger.cpp
        src/rfx/common/Logger.h
        src/rfx/main.cpp
        src/rfx/pch.h
        src/rfx/rfx.h
        src/rfx/common/Algorithm.h
        src/rfx/common/FileUtil.cpp
        src/rfx/common/FileUtil.h
        src/rfx/common/StringUtil.cpp
        src/rfx/common/StringUtil.h
        src/rfx/common/to.h
        src/rfx/application/Application.cpp
        src/rfx/application/Application.h
        src/rfx/application/Window.cpp
        src/rfx/application/Window.h
        src/rfx/application/ShaderLoader.cpp
        src/rfx/application/ShaderLoader.h
        src/rfx/graphics/GraphicsContext.cpp
        src/rfx/graphics/GraphicsContext.h
        src/rfx/graphics/GraphicsDevice.cpp
        src/rfx/graphics/GraphicsDevice.h
        src/rfx/graphics/Queue.cpp
        src/rfx/graphics/Queue.h
        src/rfx/graphics/SwapChain.cpp
        src/rfx/graphics/SwapChain.h
        src/rfx/graphics/ShaderCompiler.cpp
        src/rfx/graphics/ShaderCompiler.h
        src/rfx/graphics/VertexFormat.cpp
        src/rfx/graphics/VertexFormat.h
        src/rfx/graphics/FragmentShader.cpp
        src/rfx/graphics/FragmentShader.h
        src/rfx/graphics/Shader.cpp
        src/rfx/graphics/Shader.h
        src/rfx/graphics/ShaderProgram.cpp
        src/rfx/graphics/ShaderProgram.h
        src/rfx/graphics/VertexShader.cpp
        src/rfx/graphics/VertexShader.h
        src/rfx/graphics/Buffer.cpp
        src/rfx/graphics/Buffer.h
        src/rfx/graphics/VertexBuffer.cpp
        src/rfx/graphics/VertexBuffer.h
        src/rfx/graphics/CommandBuffer.cpp
        src/rfx/graphics/CommandBuffer.h
        src/rfx/graphics/IndexBuffer.cpp
        src/rfx/graphics/IndexBuffer.h
        src/rfx/application/Texture2DLoader.cpp
        src/rfx/application/Texture2DLoader.h
        test/ColoredQuad.cpp
        test/ColoredQuad.h
        test/TexturedQuad.cpp
        test/TexturedQuad.h
        src/rfx/graphics/Texture2D.cpp
        src/rfx/graphics/Texture2D.h src/rfx/graphics/Image.cpp src/rfx/graphics/Image.h src/rfx/graphics/DepthBuffer.cpp src/rfx/graphics/DepthBuffer.h src/rfx/graphics/GraphicsDeviceDesc.h src/rfx/graphics/QueueFamilyDesc.h src/rfx/graphics/SwapChainDesc.h src/rfx/graphics/SurfaceDesc.h)

#target_precompile_headers(${PROJECT_NAME} PRIVATE "src/rfx/pch.h")

target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/test"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/ktx/include"
        "$ENV{VULKAN_SDK}/include"
)

target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/ktx/lib"
        $ENV{VULKAN_SDK}/lib)

target_link_libraries(${PROJECT_NAME} PRIVATE
        vulkan-1
        ktx)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

find_package(glslang CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glslang::HLSL glslang::SPIRV glslang::glslang glslang::OGLCompiler)

find_path(STB_INCLUDE_DIRS  "stb.h")
target_include_directories(${PROJECT_NAME} PRIVATE ${STB_INCLUDE_DIRS})