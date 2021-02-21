#include "rfx/pch.h"
#include "rfx/graphics/GraphicsContext.h"
#include "rfx/common/Logger.h"

using namespace rfx;
using namespace std;
namespace ranges = ranges;

// ---------------------------------------------------------------------------------------------------------------------

static constexpr uint16_t REQUIRED_VERSION_MAJOR = 1;
static constexpr uint16_t REQUIRED_VERSION_MINOR = 2;
static constexpr uint32_t REQUIRED_VERSION = VK_MAKE_VERSION(REQUIRED_VERSION_MAJOR, REQUIRED_VERSION_MINOR, 0);

// ---------------------------------------------------------------------------------------------------------------------

static const vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};


static PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = nullptr;
static PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = nullptr;

// ---------------------------------------------------------------------------------------------------------------------

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void*) {

    static unordered_map<VkDebugUtilsMessageSeverityFlagBitsEXT, LogLevel> logLevelMap = {
        { VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, LogLevel::DEBUG },
        { VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, LogLevel::INFO },
        { VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, LogLevel::WARNING },
        { VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, LogLevel::ERROR },
    };

    RFX_LOG(logLevelMap[messageSeverity]) << pCallbackData->pMessage;

    return VK_FALSE;
}

// ---------------------------------------------------------------------------------------------------------------------

GraphicsContext::GraphicsContext(shared_ptr<Window> window)
    : window(move(window))
{
#ifdef _DEBUG
    validationEnabled = true;
    requiredLayers.insert(requiredLayers.end(), validationLayers.begin(), validationLayers.end());
#endif // _DEBUG
}

// ---------------------------------------------------------------------------------------------------------------------

GraphicsContext::~GraphicsContext()
{
    vkDestroySurfaceKHR(instance, presentSurface, nullptr);
    if (validationEnabled) {
        vkDestroyDebugUtilsMessenger(instance, debugMessenger, nullptr);
    }
    vkDestroyInstance(instance, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::initialize()
{
//    dumpExtensions();
    checkValidationSupport();

    createVulkanInstance();
    createValidationLoggerAdapter();
    createPresentationSurface();
    queryPhysicalDevices();
    queryDeviceGroups();
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::checkValidationSupport() const
{
    if (!validationEnabled) {
        return;
    }

    uint32_t instanceLayerCount = 0;
    vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);

    vector<VkLayerProperties> instanceLayers(instanceLayerCount);
    vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : instanceLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        RFX_CHECK_STATE(layerFound == true, "Validation layer unavailable: " + layerName);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::createVulkanInstance()
{
    VkApplicationInfo applicationInfo {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "rfx Demo",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "rfx",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_2
    };

    uint32_t extensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    vector<const char*> extensions(glfwExtensions, glfwExtensions + extensionCount);
    if (validationEnabled) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    VkInstanceCreateInfo instanceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &applicationInfo,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data()
    };
    instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
    instanceCreateInfo.ppEnabledLayerNames = requiredLayers.data();

    ThrowIfFailed(vkCreateInstance(
        &instanceCreateInfo,
        nullptr,
        &instance));
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::dumpExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    ostringstream oss;
    oss << "Available extensions:\n";

    for (const auto& extension : extensions) {
        oss << '\t' << extension.extensionName << '\n';
    }

    RFX_LOG_INFO << oss.str();
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::createValidationLoggerAdapter()
{
    if (!validationEnabled) {
        return;
    }

    vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    RFX_CHECK_STATE(vkCreateDebugUtilsMessenger != nullptr, "vkCreateDebugUtilsMessengerEXT unavailable");

    vkDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    RFX_CHECK_STATE(vkDestroyDebugUtilsMessenger != nullptr, "vkDestroyDebugUtilsMessengerEXT unavailable");

    VkDebugUtilsMessageTypeFlagsEXT messageTypeFlags =
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    VkDebugUtilsMessageSeverityFlagsEXT messageSeverityFlags =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    LogLevel logLevel = Logger::getLogLevel();
    if (logLevel > LogLevel::ERROR) {
        messageSeverityFlags |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    }
    if (logLevel > LogLevel::WARNING) {
        messageSeverityFlags |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    }
    if (logLevel > LogLevel::INFO) {
        messageSeverityFlags |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    }
    if (logLevel > LogLevel::DEBUG) {
        messageTypeFlags |= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = messageSeverityFlags,
        .messageType = messageTypeFlags,
        .pfnUserCallback = debugCallback
    };

    ThrowIfFailed(vkCreateDebugUtilsMessenger(
        instance,
        &createInfo,
        nullptr,
        &debugMessenger));
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::createPresentationSurface()
{
#ifdef _WINDOWS
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hinstance = GetModuleHandle(nullptr),
        .hwnd = static_cast<HWND>(window->getHandle())
    };

    ThrowIfFailed(vkCreateWin32SurfaceKHR(
        instance,
        &surfaceCreateInfo,
        nullptr,
        &presentSurface));
#endif // _WINDOWS
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::queryPhysicalDevices()
{
    uint32_t physicalDeviceCount = 0;
    ThrowIfFailed(vkEnumeratePhysicalDevices(
        instance,
        &physicalDeviceCount,
        nullptr));

    vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    ThrowIfFailed(vkEnumeratePhysicalDevices(
        instance,
        &physicalDeviceCount,
        physicalDevices.data()));

    for (const auto& physicalDevice : physicalDevices) {
        GraphicsDeviceDesc deviceDesc {
            .physicalDevice = physicalDevice
        };

        queryProperties(physicalDevice, &deviceDesc);
        queryFeatures(physicalDevice, &deviceDesc);
        queryExtensions(physicalDevice, &deviceDesc);
        queryQueueFamilies(physicalDevice, &deviceDesc);

        deviceDescs[physicalDevice] = deviceDesc;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::queryDeviceGroups()
{
    uint32_t deviceGroupCount = 0;
    ThrowIfFailed(vkEnumeratePhysicalDeviceGroups(
        instance,
        &deviceGroupCount,
        nullptr));

    deviceGroups.resize(deviceGroupCount);

    for (uint32_t i = 0; i < deviceGroupCount; ++i) {
        deviceGroups[i] = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES,
            .pNext = nullptr
        };
    }

    vkEnumeratePhysicalDeviceGroups(instance, &deviceGroupCount, &deviceGroups[0]);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::queryProperties(VkPhysicalDevice physicalDevice, GraphicsDeviceDesc* deviceDesc)
{
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceDesc->properties);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceDesc->memoryProperties);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::queryFeatures(VkPhysicalDevice physicalDevice, GraphicsDeviceDesc* deviceDesc)
{
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceDesc->features);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::queryExtensions(VkPhysicalDevice physicalDevice, GraphicsDeviceDesc* deviceDesc)
{
    uint32_t extensionCount = 0;

    ThrowIfFailed(vkEnumerateDeviceExtensionProperties(
        physicalDevice,
        nullptr,
        &extensionCount,
        nullptr));

    if (extensionCount == 0) {
        return;
    }

    deviceDesc->extensions.resize(extensionCount);

    ThrowIfFailed(vkEnumerateDeviceExtensionProperties(
        physicalDevice,
        nullptr,
        &extensionCount,
        &deviceDesc->extensions[0]));
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::queryQueueFamilies(VkPhysicalDevice physicalDevice, GraphicsDeviceDesc* deviceDesc)
{
    uint32_t queueFamiliesCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
    if (queueFamiliesCount == 0) {
        return;
    }

    vector<VkQueueFamilyProperties> queueFamilies;
    queueFamilies.resize(queueFamiliesCount);

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, &queueFamilies[0]);
    RFX_CHECK_STATE(queueFamiliesCount > 0, "Failed to query queue families");

    deviceDesc->queueFamilies = getQueueFamilyDescriptions(physicalDevice, queueFamilies);
}

// ---------------------------------------------------------------------------------------------------------------------

vector<QueueFamilyDesc> GraphicsContext::getQueueFamilyDescriptions(
    VkPhysicalDevice physicalDevice,
    const vector<VkQueueFamilyProperties>& queueFamilies) const
{
    vector<QueueFamilyDesc> queueFamilyDescs;
    queueFamilyDescs.resize(queueFamilies.size());

    for (size_t i = 0, count = queueFamilies.size(); i < count; ++i)
    {
        const VkQueueFamilyProperties& properties = queueFamilies[i];

        QueueFamilyDesc& desc = queueFamilyDescs[i];
        desc.familyIndex = static_cast<uint32_t>(i);
        desc.properties = properties;
        desc.priorities = {1.0f };
        desc.supportsPresentation = isPresentationSupported(physicalDevice, desc.familyIndex);

        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            desc.flagNames.emplace_back("VK_QUEUE_GRAPHICS_BIT");
        }

        if (properties.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            desc.flagNames.emplace_back("VK_QUEUE_COMPUTE_BIT");
        }

        if (properties.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            desc.flagNames.emplace_back("VK_QUEUE_TRANSFER_BIT");
        }

        if (properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
            desc.flagNames.emplace_back("VK_QUEUE_SPARSE_BINDING_BIT");
        }

        if (properties.queueFlags & VK_QUEUE_PROTECTED_BIT) {
            desc.flagNames.emplace_back("VK_QUEUE_PROTECTED_BIT");
        }
    }

    return queueFamilyDescs;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GraphicsContext::isPresentationSupported(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) const
{
    VkBool32 presentationSupported = VK_FALSE;

    ThrowIfFailed(vkGetPhysicalDeviceSurfaceSupportKHR(
        physicalDevice,
        queueFamilyIndex,
        presentSurface,
        &presentationSupported));

    return presentationSupported == VK_TRUE;
}

// ---------------------------------------------------------------------------------------------------------------------


shared_ptr<GraphicsDevice> GraphicsContext::createGraphicsDevice(
    const VkPhysicalDeviceFeatures& features,
    const vector<string>& extensions,
    const vector<VkQueueFlagBits>& queueCapabilities)
{
    VkPhysicalDevice physicalDevice =
        findFirstMatchingPhysicalDevice(features, extensions, queueCapabilities);

    RFX_CHECK_STATE(physicalDevice != nullptr, "No suitable device available");

    return createLogicalDevice(physicalDevice, features, extensions, queueCapabilities);
}

// ---------------------------------------------------------------------------------------------------------------------

VkPhysicalDevice GraphicsContext::findFirstMatchingPhysicalDevice(
    const VkPhysicalDeviceFeatures& features,
    const vector<string>& extensions,
    const vector<VkQueueFlagBits>& queueCapabilities) const
{
    VkPhysicalDevice matchingDevice = VK_NULL_HANDLE;

    for (const VkPhysicalDeviceGroupProperties& deviceGroup : deviceGroups) {
        matchingDevice = deviceGroup.physicalDevices[0];

        for (uint32_t i = 0; i < deviceGroup.physicalDeviceCount; ++i) {
            VkPhysicalDevice currentDevice = deviceGroup.physicalDevices[i];
            const auto& it = deviceDescs.find(currentDevice);
            RFX_CHECK_STATE(it != deviceDescs.end(), "Internal error");
            const GraphicsDeviceDesc& deviceDesc = it->second;

            if (!isMatching(deviceDesc, features, extensions, queueCapabilities)) {
                matchingDevice = nullptr;
                break;
            }
        }

        if (matchingDevice != nullptr) {
            return matchingDevice;
        }
    }

    for (const auto& it : deviceDescs) {
        if (isMatching(it.second, features, extensions, queueCapabilities)) {
            matchingDevice = it.second.physicalDevice;
            break;
        }
    }

    return matchingDevice;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GraphicsContext::isMatching(
    const GraphicsDeviceDesc& desc,
    const VkPhysicalDeviceFeatures& features,
    const vector<string>& extensions,
    const vector<VkQueueFlagBits>& queueCapabilities) const
{
    return isDiscreteGPU(desc)
           && hasRequiredAPIVersion(desc)
           && hasRequiredFeatures(desc, features)
           && hasRequiredExtensions(desc, extensions)
           && hasRequiredQueueCapabilities(desc, queueCapabilities)
           && supportsSwapChain(desc);
}

// ---------------------------------------------------------------------------------------------------------------------

bool GraphicsContext::isDiscreteGPU(const GraphicsDeviceDesc& desc) const
{
    return desc.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GraphicsContext::hasRequiredAPIVersion(const GraphicsDeviceDesc& deviceDesc) const
{
    return deviceDesc.properties.apiVersion >= REQUIRED_VERSION;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GraphicsContext::hasRequiredFeatures(
    const GraphicsDeviceDesc& deviceDesc,
    const VkPhysicalDeviceFeatures& requiredFeatures) const
{
    const size_t structSize = sizeof(VkPhysicalDeviceFeatures);
    const size_t flagSize = sizeof(VkBool32);
    const size_t flagCount = structSize / flagSize;

    auto actualFeature = reinterpret_cast<const VkBool32*>(&deviceDesc.features);
    auto requiredFeature = reinterpret_cast<const VkBool32*>(&requiredFeatures);

    for (size_t i = 0; i < flagCount; ++i) {
        if (requiredFeature[i] == VK_TRUE && actualFeature[i] != VK_TRUE) {
            return false;
        }
    }

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GraphicsContext::hasRequiredExtensions(
    const GraphicsDeviceDesc& deviceDesc,
    const vector<string>& requiredExtensions) const
{
    const vector<VkExtensionProperties>& actualExtensions = deviceDesc.extensions;

    for (const auto& requiredExtension : requiredExtensions) {
        auto pos = ranges::find_if(actualExtensions,
            [&requiredExtension](const VkExtensionProperties& actualExtension) {
                return actualExtension.extensionName == requiredExtension;
            });

        if (pos == actualExtensions.end()) {
            return false;
        }
    }

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GraphicsContext::hasRequiredQueueCapabilities(
    const GraphicsDeviceDesc& deviceDesc,
    const vector<VkQueueFlagBits>& requiredQueueCapabilities) const
{
    auto containsRequiredCapability =
        [&deviceDesc](VkQueueFlagBits requiredCapability) {
            const size_t index = getQueueFamilyIndex(deviceDesc, requiredCapability);
            if (index == static_cast<size_t>(-1)) {
                RFX_LOG_WARNING << "Requested queue capability not available: " << to_string(requiredCapability);
                return false;
            }
            return true;
        };

    return ranges::all_of(requiredQueueCapabilities, containsRequiredCapability);
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t GraphicsContext::getQueueFamilyIndex(
    const GraphicsDeviceDesc& deviceDesc,
    VkQueueFlags desiredCapabilities)
{
    for (uint32_t i = 0, count = deviceDesc.queueFamilies.size(); i < count; ++i) {
        const auto& currentQueueFamily = deviceDesc.queueFamilies[i];
        if (currentQueueFamily.properties.queueCount > 0
                && currentQueueFamily.properties.queueFlags & desiredCapabilities) {
            return i;
        }
    }

    return -1;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GraphicsContext::supportsSwapChain(const GraphicsDeviceDesc& desc) const
{
    const SurfaceDesc surfaceDesc = querySurfaceDesc(desc.physicalDevice, presentSurface);

    return !surfaceDesc.formats.empty()
        && !surfaceDesc.presentModes.empty();
}

// ---------------------------------------------------------------------------------------------------------------------

SurfaceDesc GraphicsContext::querySurfaceDesc(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    SurfaceDesc surfaceDesc {};

    ThrowIfFailed(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physicalDevice,
        surface,
        &surfaceDesc.capabilities));

    uint32_t formatCount;
    ThrowIfFailed(vkGetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice,
        surface,
        &formatCount,
        nullptr));

    if (formatCount != 0) {
        surfaceDesc.formats.resize(formatCount);
        ThrowIfFailed(vkGetPhysicalDeviceSurfaceFormatsKHR(
            physicalDevice,
            surface,
            &formatCount,
            surfaceDesc.formats.data()));
    }

    uint32_t presentationModesCount = 0;
    ThrowIfFailed(vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice,
        surface,
        &presentationModesCount,
        nullptr));

    if (presentationModesCount != 0) {
        surfaceDesc.presentModes.resize(presentationModesCount);
        ThrowIfFailed(vkGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice,
            surface,
            &presentationModesCount,
            surfaceDesc.presentModes.data()));
    }

    return surfaceDesc;
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<GraphicsDevice> GraphicsContext::createLogicalDevice(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceFeatures& features,
    const vector<string>& extensions,
    const vector<VkQueueFlagBits>& queueCapabilities) const
{
    vector<QueueFamilyDesc> selectedQueueFamilies;
    uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
    uint32_t presentQueueFamilyIndex = UINT32_MAX;
    selectQueueFamilies(
        physicalDevice,
        queueCapabilities,
        selectedQueueFamilies,
        graphicsQueueFamilyIndex,
        presentQueueFamilyIndex);

    vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    queueCreateInfos.reserve(selectedQueueFamilies.size());

    for (const auto& queueFamily : selectedQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queueFamily.familyIndex,
            .queueCount = 1, // queueFamily.priorities.size()
            .pQueuePriorities = queueFamily.priorities.data()
        };

        queueCreateInfos.push_back(queueCreateInfo);
    }

    vector<const char*> extensionsAsChars;
    ranges::transform(extensions, back_inserter(extensionsAsChars),
        [](const string& name) -> const char* { return name.c_str(); });

    VkDeviceCreateInfo deviceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensionsAsChars.size()),
        .ppEnabledExtensionNames = extensionsAsChars.data(),
        .pEnabledFeatures = &features
    };

    VkDeviceGroupDeviceCreateInfoKHR deviceGroupInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO_KHR
    };

    const int deviceGroupIndex = getDeviceGroupIndex(physicalDevice);
    if (deviceGroupIndex != -1) {
        const VkPhysicalDeviceGroupProperties& deviceGroup = deviceGroups[deviceGroupIndex];
        if (deviceGroup.physicalDeviceCount > 1) {
            deviceGroupInfo.physicalDeviceCount = deviceGroup.physicalDeviceCount;
            deviceGroupInfo.pPhysicalDevices = deviceGroup.physicalDevices;
            deviceCreateInfo.pNext = &deviceGroupInfo;
        }
    }

    VkDevice logicalDevice = VK_NULL_HANDLE;
    ThrowIfFailed(vkCreateDevice(
        physicalDevice,
        &deviceCreateInfo,
        nullptr,
        &logicalDevice));

    VkQueue vkQueue = VK_NULL_HANDLE;
    vkGetDeviceQueue(logicalDevice, graphicsQueueFamilyIndex, 0, &vkQueue);
    RFX_CHECK_STATE(vkQueue != VK_NULL_HANDLE, "Failed to get graphics queue");
    const auto graphicsQueue = make_shared<Queue>(vkQueue, graphicsQueueFamilyIndex);

    vkQueue = VK_NULL_HANDLE;
    vkGetDeviceQueue(logicalDevice, presentQueueFamilyIndex, 0, &vkQueue);
    RFX_CHECK_STATE(vkQueue != VK_NULL_HANDLE, "Failed to get presentation queue");
    const auto presentQueue = make_shared<Queue>(vkQueue, presentQueueFamilyIndex);

    const auto& it = deviceDescs.find(physicalDevice);
    RFX_CHECK_STATE(it != deviceDescs.end(), "Internal error");
    const GraphicsDeviceDesc& deviceDesc = it->second;
    shared_ptr<GraphicsDevice> graphicsDevice = make_shared<GraphicsDevice>(
        deviceDesc,
        physicalDevice,
        logicalDevice,
        graphicsQueue,
        presentQueue,
        presentSurface);

    return graphicsDevice;
}

// ---------------------------------------------------------------------------------------------------------------------

int GraphicsContext::getDeviceGroupIndex(VkPhysicalDevice physicalDevice) const
{
    for (size_t i = 0, count = deviceGroups.size(); i < count; ++i) {
        const VkPhysicalDeviceGroupProperties& currentGroup = deviceGroups[i];
        for (VkPhysicalDevice currentGroupDevice : currentGroup.physicalDevices) {
            if (currentGroupDevice == physicalDevice) {
                return static_cast<int>(i);
            }
        }
    }

    return -1;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::selectQueueFamilies(
    VkPhysicalDevice physicalDevice,
    const vector<VkQueueFlagBits>& queueCapabilities,
    vector<QueueFamilyDesc>& outSelectedQueueFamilies,
    uint32_t& outGraphicsQueueFamilyIndex,
    uint32_t& outPresentQueueFamilyIndex) const
{
    outSelectedQueueFamilies.clear();
    outGraphicsQueueFamilyIndex = UINT32_MAX;
    outPresentQueueFamilyIndex = UINT32_MAX;

    // #1: try to find family with graphics and presentation capability
    const auto& it = deviceDescs.find(physicalDevice);
    RFX_CHECK_STATE(it != deviceDescs.end(), "Internal error");
    const GraphicsDeviceDesc& deviceDesc = it->second;
    const vector<QueueFamilyDesc>& queueFamilies = deviceDesc.queueFamilies;
    for (size_t i = 0, count = queueFamilies.size(); i < count; ++i) {
        if (queueFamilies[i].properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            if (outGraphicsQueueFamilyIndex == UINT32_MAX) {
                outGraphicsQueueFamilyIndex = static_cast<uint32_t>(i);
            }

            if (queueFamilies[i].supportsPresentation) {
                outGraphicsQueueFamilyIndex = static_cast<uint32_t>(i);
                outPresentQueueFamilyIndex = static_cast<uint32_t>(i);
                break;
            }
        }
    }

    if (outPresentQueueFamilyIndex == UINT32_MAX) {
        // #2: try to find separate family for presentation
        for (uint32_t i = 0, count = queueFamilies.size(); i < count; ++i) {
            if (queueFamilies[i].supportsPresentation) {
                outPresentQueueFamilyIndex = i;
                break;
            }
        }
    }
    RFX_CHECK_STATE(outPresentQueueFamilyIndex != UINT32_MAX, "No presentation queue available");

    outSelectedQueueFamilies.push_back(queueFamilies[outGraphicsQueueFamilyIndex]);
    if (outGraphicsQueueFamilyIndex != outPresentQueueFamilyIndex) {
        outSelectedQueueFamilies.push_back(queueFamilies[outPresentQueueFamilyIndex]);
    }

    // Check for additional requested capabilities
    for (VkQueueFlagBits currentCapability : queueCapabilities) {
        const auto hasCapability =
            [currentCapability](const QueueFamilyDesc& queueFamilyDesc) {
                return queueFamilyDesc.properties.queueFlags & currentCapability;
            };

        // #1: check if capability exists in already selected queues
        const auto it = ranges::find_if(outSelectedQueueFamilies, hasCapability);
        if (it != outSelectedQueueFamilies.end()) {
            continue;
        }

        // #2: check if capability exists in any other queue and add it to selected ones
        auto pos = ranges::find_if(queueFamilies, hasCapability);
        RFX_CHECK_STATE(pos != queueFamilies.end(), "Required capability not found: " + to_string(currentCapability));
        outSelectedQueueFamilies.push_back(*pos);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

VkInstance GraphicsContext::getInstance() const
{
    return instance;
}

// ---------------------------------------------------------------------------------------------------------------------

