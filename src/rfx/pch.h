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
#include <numbers>
#include <numeric>

//#ifdef _WINDOWS
//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
//#endif

#ifdef _WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif // _WINDOWS

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_SSE2
//#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <SPIRV/GlslangToSpv.h>
#include <ktx.h>
#include <imgui.h>
#include <fmt/core.h>

#include "rfx.h"

