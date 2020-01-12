// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#pragma once

#include "res/targetver.h"

// Windows Header Files
#ifdef _WINDOWS
#define NOMINMAX            // Don't let Windows define min() or max()
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <wrl.h>
#endif // _WINDOWS

// DirectX Header Files
#ifdef _WINDOWS
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#endif // _WINDOWS

// Vulkan Header Files
#ifdef _WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif // _WINDOWS
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

// GLM Header Files
#define GLM_FORCE_SSE2
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// glslang Header Files
#include "SPIRV/GlslangToSpv.h"

// JsonCpp Header Files
#include <json/json.h>

// STL Header Files
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <functional>

// Assimp Header Files
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>     
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

// RFX Header Files
#include "rfx/rfx.h"
