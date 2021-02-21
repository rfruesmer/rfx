#pragma once

#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <filesystem>
#include <regex>
#include <algorithm>
#include <ranges>

#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#ifdef _WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif // _WINDOWS

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifdef _WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif // _WINDOWS

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SPIRV/GlslangToSpv.h>
#include <ktx.h>

#include "rfx.h"

