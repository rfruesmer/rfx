#include "rfx/pch.h"
#include "rfx/graphics/GraphicsContext.h"
#include "rfx/core/Algorithm.h"
#include <unordered_set>

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

static const uint16_t REQUIRED_VERSION_MAJOR = 1;
static const uint16_t REQUIRED_VERSION_MINOR = 1;
static const uint32_t REQUIRED_VERSION = VK_MAKE_VERSION(REQUIRED_VERSION_MAJOR, REQUIRED_VERSION_MINOR, 0);

// ---------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG

VkBool32 GraphicsContext::DebugReportCallback(VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t object,
    size_t location,
    int32_t messageCode,
    const char* pLayerPrefix,
    const char* pMessage,
    void* pUserData)
{
    RFX_LOG_ERROR << pMessage;

#ifdef _MSC_VER
    __debugbreak();
#endif // _MSC_VER

    return VK_FALSE;
}

#endif // _DEBUG

// ---------------------------------------------------------------------------------------------------------------------

GraphicsContext::~GraphicsContext()
{
    dispose();
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::dispose()
{
    if (presentationSurface != nullptr)
    {
        vkDestroySurfaceKHR(vkInstance, presentationSurface, nullptr);
        presentationSurface = nullptr;
    }

    if (vkInstance != nullptr)
    {
        vkDestroyInstance(vkInstance, nullptr);
        vkInstance = nullptr;
    }
       
    if (vkLibrary == nullptr)
    {
#ifdef _WINDOWS
        FreeLibrary(static_cast<HMODULE>(vkLibrary));
#else
        dlclose(vkLibrary);
#endif // _WINDOWS

        vkLibrary = nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::initialize(const string& applicationName, 
                                 const shared_ptr<Window>& window)
{
    initialize(applicationName, window, vector<string>());
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::initialize(const string& applicationName,
                                 const shared_ptr<Window>& window,
                                 const vector<string>& instanceExtensions)
{
    RFX_CHECK_ARGUMENT(window != nullptr && window->getHandle() != nullptr, "window");

    loadVulkanLibrary();
    loadVulkanLoaderFunctions();
    loadGlobalFunctions();
    queryInstanceExtensions();
    checkThatRequestedInstanceExtensionsAreAvailable(instanceExtensions);
    createVulkanInstance(applicationName, instanceExtensions);
    loadInstanceFunctions();
    createPresentationSurface(window);
    queryPhysicalDevices(window);
    queryDeviceGroups();
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::loadVulkanLibrary()
{
#ifdef _WINDOWS
    vkLibrary = LoadLibrary(L"vulkan-1.dll");
#else
    vkLibrary = dlopen("libvulkan.so.1", RTLD_NOW);
#endif // _WINDOWS

    RFX_CHECK_STATE(vkLibrary != nullptr, 
        "Failed to load Vulkan runtime library");
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::loadVulkanLoaderFunctions()
{
#ifdef _WINDOWS
#define LOADFUNCTION GetProcAddress
#define LIBRARYTYPE HMODULE
#else
#define LOADFUNCTION dlsym
#define LIBRARYTYPE void*
#endif // _WINDOWS

#define LOAD_VULKANLOADER_FUNCTION(name)                                                 \
    name = (PFN_##name) LOADFUNCTION((LIBRARYTYPE)vkLibrary, #name);                 \
    RFX_CHECK_STATE(name != nullptr, string("Failed to load function: ") + (#name));

    LOAD_VULKANLOADER_FUNCTION(vkGetInstanceProcAddr);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::loadGlobalFunctions()
{
#define LOAD_GLOBAL_VULKAN_FUNCTION(name)                                                \
    name = (PFN_##name)vkGetInstanceProcAddr(nullptr, #name);                            \
    RFX_CHECK_STATE(name != nullptr, string("Failed to load function: ") + (#name));

    LOAD_GLOBAL_VULKAN_FUNCTION(vkEnumerateInstanceVersion);
    LOAD_GLOBAL_VULKAN_FUNCTION(vkEnumerateInstanceExtensionProperties);
    LOAD_GLOBAL_VULKAN_FUNCTION(vkEnumerateInstanceLayerProperties);
    LOAD_GLOBAL_VULKAN_FUNCTION(vkCreateInstance);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::queryInstanceExtensions()
{
    uint32_t extensionCount = 0;

    VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    RFX_CHECK_STATE(result == VK_SUCCESS && extensionCount > 0, 
        "Failed to retrieve instance extension count");

    vector<VkExtensionProperties> extensions;
    extensions.resize(extensionCount);

    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, &extensions[0]);
    RFX_CHECK_STATE(result == VK_SUCCESS && extensionCount > 0,
        "Failed to retrieve instance extension properties");

    for (const auto& extension : extensions)
    {
        instanceExtensions[extension.extensionName] = extension;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::checkThatRequestedInstanceExtensionsAreAvailable(const vector<string>& requestedExtensions)
{
    vector<string> missingExtensions;

    for (const auto& requestedExtension : requestedExtensions)
    {
        if (instanceExtensions.find(requestedExtension) == instanceExtensions.end())
        {
            missingExtensions.push_back(requestedExtension);
        }
    }

    if (!missingExtensions.empty())
    {
        string errorMessage = "The following requested extensions aren't supported:\n";

        for (const auto& extension : missingExtensions)
        {
            errorMessage += "- " + extension + "\n";
        }

        RFX_THROW(errorMessage);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::createVulkanInstance(const string& applicationName,
    const vector<string>& requestedExtensions)
{
    uint32_t apiVersion;
    VkResult result = vkEnumerateInstanceVersion(&apiVersion);
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to enumerate instance version");
    const uint32_t loaderMajorVersion = VK_VERSION_MAJOR(apiVersion);
    const uint32_t loaderMinorVersion = VK_VERSION_MINOR(apiVersion);
    RFX_LOG_INFO << "Loader/Runtime support detected for Vulkan "
        << loaderMajorVersion << "." << loaderMinorVersion;

    RFX_CHECK_STATE (loaderMajorVersion > REQUIRED_VERSION_MAJOR ||
        (loaderMajorVersion == REQUIRED_VERSION_MAJOR && loaderMinorVersion >= REQUIRED_VERSION_MINOR),
        "Required Vulkan version not supported");

    vector<const char*> layers;

#ifdef _DEBUG
    layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif  // _DEBUG

    vector<const char*> copyOfExtensions;
    rfx::transform(requestedExtensions, back_inserter(copyOfExtensions),
        [](const string& name) -> const char* { return name.c_str(); });

    copyOfExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
    copyOfExtensions.emplace_back(
#ifdef VK_USE_PLATFORM_WIN32_KHR
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif VK_USE_PLATFORM_XCB_KHR
        VK_KHR_XCB_SURFACE_EXTENSION_NAME
#elif VK_USE_PLATFORM_XLIB_KHR
        VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#endif // _WINDOWS
    );

#ifdef _DEBUG
    copyOfExtensions.push_back("VK_EXT_debug_report");
#endif // _DEBUG

    VkApplicationInfo applicationInfo = {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        nullptr,
        applicationName.c_str(),
        VK_MAKE_VERSION(1, 0, 0),
        "rfx",
        VK_MAKE_VERSION(1, 0, 0),
        REQUIRED_VERSION
    };

    VkInstanceCreateInfo instanceCreateInfo = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        nullptr,
        0,
        &applicationInfo,
        static_cast<uint32_t>(layers.size()),
        &layers[0],
        static_cast<uint32_t>(copyOfExtensions.size()),
        copyOfExtensions.data()
    };

    result = vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance);
    RFX_CHECK_STATE(result == VK_SUCCESS && vkInstance != nullptr,
        "Failed to create Vulkan instance");
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::loadInstanceFunctions()
{
#define LOAD_INSTANCE_VULKAN_FUNCTION(name)                                              \
    name = (PFN_##name)vkGetInstanceProcAddr(vkInstance, #name);                     \
    RFX_CHECK_STATE(name != nullptr, string("Failed to load function: ") + (#name));

    LOAD_INSTANCE_VULKAN_FUNCTION(vkDestroyInstance);
    LOAD_INSTANCE_VULKAN_FUNCTION(vkEnumeratePhysicalDevices);
    LOAD_INSTANCE_VULKAN_FUNCTION(vkEnumerateDeviceExtensionProperties);
    LOAD_INSTANCE_VULKAN_FUNCTION(vkEnumeratePhysicalDeviceGroups);
    LOAD_INSTANCE_VULKAN_FUNCTION(vkGetPhysicalDeviceProperties);
    LOAD_INSTANCE_VULKAN_FUNCTION(vkGetPhysicalDeviceFormatProperties);
    LOAD_INSTANCE_VULKAN_FUNCTION(vkGetPhysicalDeviceMemoryProperties);
    LOAD_INSTANCE_VULKAN_FUNCTION(vkGetPhysicalDeviceFeatures);
    LOAD_INSTANCE_VULKAN_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);
    LOAD_INSTANCE_VULKAN_FUNCTION(vkCreateDevice);
    LOAD_INSTANCE_VULKAN_FUNCTION(vkGetDeviceProcAddr);

    LOAD_INSTANCE_VULKAN_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);
    LOAD_INSTANCE_VULKAN_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    LOAD_INSTANCE_VULKAN_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
    LOAD_INSTANCE_VULKAN_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
    LOAD_INSTANCE_VULKAN_FUNCTION(vkDestroySurfaceKHR);

#ifdef _WINDOWS
    LOAD_INSTANCE_VULKAN_FUNCTION(vkCreateWin32SurfaceKHR);
#else
static_assert(false, "not implemented");
#endif // _WINDOWS

#ifdef _DEBUG
    LOAD_INSTANCE_VULKAN_FUNCTION(vkCreateDebugReportCallbackEXT);
    LOAD_INSTANCE_VULKAN_FUNCTION(vkDebugReportMessageEXT);
    LOAD_INSTANCE_VULKAN_FUNCTION(vkDestroyDebugReportCallbackEXT);

    VkDebugReportCallbackCreateInfoEXT callbackCreateInfo;
    callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    callbackCreateInfo.pNext = nullptr;
    callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
        VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    callbackCreateInfo.pfnCallback = &DebugReportCallback;
    callbackCreateInfo.pUserData = nullptr;

    /* Register the callback */
    VkDebugReportCallbackEXT callback;
    VkResult result = vkCreateDebugReportCallbackEXT(vkInstance, &callbackCreateInfo, nullptr, &callback);
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to enable debug reporting");
#endif // _DEBUG
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::createPresentationSurface(const shared_ptr<Window>& window)
{
#ifdef VK_USE_PLATFORM_WIN32_KHR
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
        VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        nullptr,
        0,
        GetModuleHandle(nullptr),
        static_cast<HWND>(window->getHandle())
    };
#else
    static_assert(false, "not implemented");
#endif

    const VkResult result = vkCreateWin32SurfaceKHR(vkInstance, &surfaceCreateInfo, nullptr, &presentationSurface);
    RFX_CHECK_STATE(result == VK_SUCCESS && presentationSurface != nullptr,
        "Failed to create presentation surface");
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::queryPhysicalDevices(const shared_ptr<Window>& window)
{
    uint32_t deviceCount = 0;

    VkResult result = vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
    RFX_CHECK_STATE(result == VK_SUCCESS && deviceCount > 0, 
        "Failed to query physical device count");

    vector<VkPhysicalDevice> devices;
    devices.resize(deviceCount);

    result = vkEnumeratePhysicalDevices(vkInstance, &deviceCount, &devices[0]);
    RFX_CHECK_STATE(result == VK_SUCCESS && deviceCount > 0,
        "Failed to query physical devices");

    for (auto device : devices)
    {
        GraphicsDeviceInfo deviceInfo;
        deviceInfo.handle = device;

        queryExtensions(device, deviceInfo);
        queryFeatures(device, deviceInfo);
        queryProperties(device, deviceInfo);
        queryQueueFamilies(device, deviceInfo);
        queryPresentModes(device, window, deviceInfo);
        queryPresentationSurfaceCapabilities(device, deviceInfo);
        queryPresentationSurfaceFormats(device, deviceInfo);
        querySwapChainImageSize(device, window, deviceInfo);

        deviceInfos[device] = deviceInfo;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::queryExtensions(VkPhysicalDevice device, GraphicsDeviceInfo& outDeviceInfo) const
{
    uint32_t extensionCount = 0;

    VkResult result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to query device extension count");

    if (extensionCount == 0) {
        return;
    }

    outDeviceInfo.extensions.resize(extensionCount);

    result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, &outDeviceInfo.extensions[0]);
    RFX_CHECK_STATE(result == VK_SUCCESS && extensionCount > 0,
        "Failed to query device extensions");
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::queryFeatures(VkPhysicalDevice device, GraphicsDeviceInfo& outDeviceInfo) const
{
    vkGetPhysicalDeviceFeatures(device, &outDeviceInfo.features);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::queryProperties(VkPhysicalDevice device, GraphicsDeviceInfo& outDeviceInfo) const
{
    vkGetPhysicalDeviceProperties(device, &outDeviceInfo.properties);
    vkGetPhysicalDeviceFormatProperties(device, GraphicsDevice::DEPTHBUFFER_FORMAT, 
        &outDeviceInfo.depthBufferFormatProperties);
    vkGetPhysicalDeviceMemoryProperties(device, &outDeviceInfo.memoryProperties);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::queryQueueFamilies(VkPhysicalDevice device, GraphicsDeviceInfo& outDeviceInfo) const
{
    uint32_t queueFamiliesCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount, nullptr);
    if (queueFamiliesCount == 0) {
        return;
    }

    std::vector<VkQueueFamilyProperties> queueFamilies;
    queueFamilies.resize(queueFamiliesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount, &queueFamilies[0]);
    RFX_CHECK_STATE(queueFamiliesCount > 0,
        "Failed to query queue families");

    outDeviceInfo.queueFamilies = getQueueFamilyInfos(device, queueFamilies);
}

// ---------------------------------------------------------------------------------------------------------------------

vector<QueueFamilyInfo> GraphicsContext::getQueueFamilyInfos(VkPhysicalDevice device, 
    const vector<VkQueueFamilyProperties>& queueFamilies) const
{
    vector<QueueFamilyInfo> queueInfos;
    queueInfos.resize(queueFamilies.size());

    for (size_t i = 0, count = queueFamilies.size(); i < count; ++i)
    {
        const VkQueueFamilyProperties& properties = queueFamilies[i];

        QueueFamilyInfo& info = queueInfos[i];
        info.familyIndex = static_cast<uint32_t>(i);
        info.properties = properties;
        info.priorities = { 1.0f };
        info.supportsPresentation = isPresentationSupported(device, info.familyIndex);

        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            info.flagNames.emplace_back("VK_QUEUE_GRAPHICS_BIT");
        }

        if (properties.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            info.flagNames.emplace_back("VK_QUEUE_COMPUTE_BIT");
        }

        if (properties.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            info.flagNames.emplace_back("VK_QUEUE_TRANSFER_BIT");
        }

        if (properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
            info.flagNames.emplace_back("VK_QUEUE_SPARSE_BINDING_BIT");
        }

        if (properties.queueFlags & VK_QUEUE_PROTECTED_BIT) {
            info.flagNames.emplace_back("VK_QUEUE_PROTECTED_BIT");
        }
    }

    return queueInfos;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GraphicsContext::isPresentationSupported(VkPhysicalDevice device, uint32_t queueFamilyIndex) const
{
    VkBool32 presentationSupported = VK_FALSE;

    const VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(device, queueFamilyIndex,
        presentationSurface, &presentationSupported);

    return result == VK_SUCCESS
        && presentationSupported == VK_TRUE;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::queryPresentModes(VkPhysicalDevice device, 
                                        const shared_ptr<Window>& window,
                                        GraphicsDeviceInfo& outDeviceInfo) const
{
    outDeviceInfo.presentSurface = presentationSurface;

    uint32_t presentModesCount = 0;

    VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, 
        presentationSurface, &presentModesCount, nullptr);
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to get number of supported present modes");

    if (presentModesCount == 0) {
        return;
    }

    outDeviceInfo.presentModes.resize(presentModesCount);

    result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, presentationSurface, 
        &presentModesCount, &outDeviceInfo.presentModes[0]);
    RFX_CHECK_STATE(result == VK_SUCCESS && presentModesCount > 0,
        "Failed to get present modes");
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::queryPresentationSurfaceCapabilities(VkPhysicalDevice device, 
    GraphicsDeviceInfo& outDeviceInfo) const
{
    const VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, presentationSurface,
        &outDeviceInfo.presentSurfaceCapabilities);
    RFX_CHECK_STATE(result == VK_SUCCESS,
        "Failed to get presentation capabilities");

    const VkSurfaceCapabilitiesKHR& surfaceCaps = outDeviceInfo.presentSurfaceCapabilities;
    outDeviceInfo.presentImageCount = surfaceCaps.minImageCount + 1;
    if (surfaceCaps.maxImageCount > 0 && outDeviceInfo.presentImageCount > surfaceCaps.maxImageCount) {
        outDeviceInfo.presentImageCount = surfaceCaps.maxImageCount;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::queryPresentationSurfaceFormats(VkPhysicalDevice device,
    GraphicsDeviceInfo& outDeviceInfo) const
{
    uint32_t formatsCount = 0;
    VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, presentationSurface, &formatsCount, nullptr);
    RFX_CHECK_STATE(result == VK_SUCCESS,
        "Failed to get number of supported presentation surface formats");

    if (formatsCount == 0) {
        return;
    }

    outDeviceInfo.presentSurfaceFormats.resize(formatsCount);

    result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, presentationSurface, &formatsCount, 
        &outDeviceInfo.presentSurfaceFormats[0]);
    RFX_CHECK_STATE(result == VK_SUCCESS && formatsCount > 0,
        "Failed to get supported presentation surface formats");
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::querySwapChainImageSize(VkPhysicalDevice device,
    const shared_ptr<Window>& window,
    GraphicsDeviceInfo& outDeviceInfo) const
{
    const VkSurfaceCapabilitiesKHR& surfaceCaps = outDeviceInfo.presentSurfaceCapabilities;

    if (surfaceCaps.currentExtent.width != 0xFFFFFFFF) {
        outDeviceInfo.presentImageSize = surfaceCaps.currentExtent;
    }
    else {
        outDeviceInfo.presentImageSize = {
            static_cast<uint32_t>(window->getWidth()),
            static_cast<uint32_t>(window->getHeight())
        };

        if (outDeviceInfo.presentImageSize.width < surfaceCaps.minImageExtent.width) {
            outDeviceInfo.presentImageSize.width = surfaceCaps.minImageExtent.width;
        }
        else if (outDeviceInfo.presentImageSize.width > surfaceCaps.maxImageExtent.width) {
            outDeviceInfo.presentImageSize.width = surfaceCaps.maxImageExtent.width;
        }

        if (outDeviceInfo.presentImageSize.height < surfaceCaps.minImageExtent.height) {
            outDeviceInfo.presentImageSize.height = surfaceCaps.minImageExtent.height;
        }
        else if (outDeviceInfo.presentImageSize.height > surfaceCaps.maxImageExtent.height) {
            outDeviceInfo.presentImageSize.height = surfaceCaps.maxImageExtent.height;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsContext::queryDeviceGroups()
{
    uint32_t deviceGroupCount = 0;
    const VkResult result = vkEnumeratePhysicalDeviceGroups(vkInstance, &deviceGroupCount, nullptr);
    RFX_CHECK_STATE(result == VK_SUCCESS,
        "Failed to query physical device groups");

    deviceGroups.resize(deviceGroupCount);
    for (uint32_t i = 0; i < deviceGroupCount; ++i) {
        deviceGroups[i].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
        deviceGroups[i].pNext = nullptr;
    }
    vkEnumeratePhysicalDeviceGroups(vkInstance, &deviceGroupCount, &deviceGroups[0]);
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<GraphicsDevice> GraphicsContext::createGraphicsDevice(
    const VkPhysicalDeviceFeatures& features,
    const vector<string>& extensions, 
    const vector<VkQueueFlagBits>& queueCapabilities)
{
    vector<string> copyOfExtensions = extensions;
    copyOfExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    const VkPhysicalDevice physicalDevice = 
        findFirstMatchingPhysicalDevice(features, copyOfExtensions, queueCapabilities);
    RFX_CHECK_STATE(physicalDevice != nullptr,
        "No suitable device available");

    return createLogicalDevice(physicalDevice, features, copyOfExtensions, queueCapabilities);
}

// ---------------------------------------------------------------------------------------------------------------------

VkPhysicalDevice GraphicsContext::findFirstMatchingPhysicalDevice(
    const VkPhysicalDeviceFeatures& features,
    const vector<string>& extensions,
    const vector<VkQueueFlagBits>& queueCapabilities)
{
    VkPhysicalDevice matchingDevice = nullptr;
    
    for (const VkPhysicalDeviceGroupProperties& deviceGroup : deviceGroups)
    {
        matchingDevice = deviceGroup.physicalDevices[0];

        for (uint32_t i = 0; i < deviceGroup.physicalDeviceCount; ++i)
        {
            VkPhysicalDevice currentDevice = deviceGroup.physicalDevices[i];
            const GraphicsDeviceInfo& deviceInfo = deviceInfos[currentDevice];

            if (!isMatching(deviceInfo, features, extensions, queueCapabilities))
            {
                matchingDevice = nullptr;
                break;
            }
        }

        if (matchingDevice != nullptr) {
            return matchingDevice;
        }
    }

    for (const auto& it : deviceInfos)
    {
        if (isMatching(it.second, features, extensions, queueCapabilities))
        {
            matchingDevice = it.second.handle;
            break;
        }
    }

    return matchingDevice;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GraphicsContext::isMatching(const GraphicsDeviceInfo& deviceInfo, 
    const VkPhysicalDeviceFeatures& features,
    const vector<string>& extensions, 
    const vector<VkQueueFlagBits>& queueCapabilities) const
{
    return hasRequiredAPIVersion(deviceInfo)
        && hasFeatures(deviceInfo, features)
        && hasExtensions(deviceInfo, extensions)
        && hasQueueCapabilities(deviceInfo, queueCapabilities);
}

// ---------------------------------------------------------------------------------------------------------------------

bool GraphicsContext::hasRequiredAPIVersion(const GraphicsDeviceInfo& deviceInfo) const
{
    return deviceInfo.properties.apiVersion >= REQUIRED_VERSION;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GraphicsContext::hasFeatures(const GraphicsDeviceInfo& deviceInfo,
    const VkPhysicalDeviceFeatures& expectedFeatures) const
{
    const size_t structSize = sizeof(VkPhysicalDeviceFeatures);
    const size_t flagSize = sizeof(VkBool32);
    const size_t flagCount = structSize / flagSize;

    const VkBool32* actualFeature = reinterpret_cast<const VkBool32*>(&deviceInfo.features);
    const VkBool32* expectedFeature = reinterpret_cast<const VkBool32*>(&expectedFeatures);
    
    for (size_t i = 0; i < flagCount; ++i)
    {
        if (expectedFeature[i] == VK_TRUE && actualFeature[i] != VK_TRUE)
        {
            return false;
        }
    }

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GraphicsContext::hasExtensions(const GraphicsDeviceInfo& deviceInfo,
    const vector<string>& expectedExtensions) const
{
    const vector<VkExtensionProperties>& actualExtensions = deviceInfo.extensions;

    for (const auto& expectedExtension : expectedExtensions)
    {
        auto pos = find_if(actualExtensions.begin(), actualExtensions.end(), 
            [&expectedExtension](const VkExtensionProperties& actualExtension)
            { return actualExtension.extensionName == expectedExtension; });

        if (pos == actualExtensions.end())
        {
            return false;
        }
    }

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GraphicsContext::hasQueueCapabilities(const GraphicsDeviceInfo& deviceInfo,
    const vector<VkQueueFlagBits>& expectedQueueCapabilities) const
{
    for (VkQueueFlagBits expectedCapability : expectedQueueCapabilities)
    {
        const size_t index = getQueueFamilyIndex(deviceInfo, expectedCapability);
        if (index == static_cast<size_t>(-1))
        {
            RFX_LOG_WARNING << "Requested queue capability not available: " << to_string(expectedCapability);
            return false;
        }
    }

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t GraphicsContext::getQueueFamilyIndex(const GraphicsDeviceInfo& deviceInfo, 
    VkQueueFlags desiredCapabilities) const
{
    for (size_t i = 0, count = deviceInfo.queueFamilies.size(); i < count; ++i)
    {
        const auto& currentQueueFamily = deviceInfo.queueFamilies[i];
        if (currentQueueFamily.properties.queueCount > 0
            && currentQueueFamily.properties.queueFlags & desiredCapabilities)
        {
            return i;
        }
    }

    return -1;
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<GraphicsDevice> GraphicsContext::createLogicalDevice(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceFeatures& features,
    const vector<string>& extensions, 
    const vector<VkQueueFlagBits>& queueCapabilities)
{
    const vector<QueueFamilyInfo> selectedQueueFamilies = 
        pickQueueFamilies(physicalDevice, queueCapabilities);

    vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    queueCreateInfos.reserve(selectedQueueFamilies.size());

    for (const auto& currentQueueFamily : selectedQueueFamilies) {
        queueCreateInfos.push_back({
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            nullptr,
            0,
            currentQueueFamily.familyIndex,
            static_cast<uint32_t>(currentQueueFamily.priorities.size()),
            currentQueueFamily.priorities.data()
        });
    };

    vector<const char*> extensionsAsChars;
    rfx::transform(extensions, back_inserter(extensionsAsChars),
        [](const string& name) -> const char* { return name.c_str(); });

    VkDeviceCreateInfo deviceCreateInfo = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        nullptr,
        0,
        static_cast<uint32_t>(queueCreateInfos.size()),
        queueCreateInfos.data(),
        0,
        nullptr,
        static_cast<uint32_t>(extensionsAsChars.size()),
        extensionsAsChars.data(),
        &features
    };

    VkDeviceGroupDeviceCreateInfoKHR deviceGroupInfo = { VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO_KHR };
    const int deviceGroupIndex = getDeviceGroupIndex(physicalDevice);
    if (deviceGroupIndex != -1)
    {
        const VkPhysicalDeviceGroupProperties& deviceGroup = deviceGroups[deviceGroupIndex];
        if (deviceGroup.physicalDeviceCount > 1)
        {
            deviceGroupInfo.physicalDeviceCount = deviceGroup.physicalDeviceCount;
            deviceGroupInfo.pPhysicalDevices = deviceGroup.physicalDevices;
            deviceCreateInfo.pNext = &deviceGroupInfo;
        }
    }

    VkDevice logicalDevice = nullptr;

    const VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice);
    RFX_CHECK_STATE(result == VK_SUCCESS && logicalDevice != nullptr,
        "Failed to create logical device");

    shared_ptr<GraphicsDevice> graphicsDevice =
         make_shared<GraphicsDevice>(logicalDevice, physicalDevice, deviceInfos[physicalDevice], vkGetDeviceProcAddr);

    graphicsDevice->initialize();

    return graphicsDevice;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<QueueFamilyInfo> GraphicsContext::pickQueueFamilies(VkPhysicalDevice device,
    const vector<VkQueueFlagBits>& queueCapabilities)
{
    GraphicsDeviceInfo& deviceInfo = deviceInfos[device];
    const vector<QueueFamilyInfo>& queueFamilyInfos = deviceInfo.queueFamilies;
    for (size_t i = 0, count = queueFamilyInfos.size(); i < count; ++i)
    {
        if (queueFamilyInfos[i].properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            if (deviceInfo.graphicsQueueFamilyIndex == UINT32_MAX)
            {
                deviceInfo.graphicsQueueFamilyIndex = static_cast<uint32_t>(i);
            }

            if (queueFamilyInfos[i].supportsPresentation)
            {
                deviceInfo.graphicsQueueFamilyIndex = static_cast<uint32_t>(i);
                deviceInfo.presentQueueFamilyIndex = static_cast<uint32_t>(i);
                deviceInfo.supportsPresentation = true;
                break;
            }
        }
    }

    if (deviceInfo.presentQueueFamilyIndex == UINT32_MAX)
    {
        for (size_t i = 0, count = queueFamilyInfos.size(); i < count; ++i)
        {
            if (queueFamilyInfos[i].supportsPresentation)
            {
                deviceInfo.presentQueueFamilyIndex = static_cast<uint32_t>(i);
                deviceInfo.supportsPresentation = true;
                break;
            }
        }
    }
        
    vector<QueueFamilyInfo> infos;
    infos.push_back(queueFamilyInfos[deviceInfo.graphicsQueueFamilyIndex]);
    if (deviceInfo.graphicsQueueFamilyIndex != deviceInfo.presentQueueFamilyIndex
            && deviceInfo.supportsPresentation)
    {
        infos.push_back(queueFamilyInfos[deviceInfo.presentQueueFamilyIndex]);
    }

    for (VkQueueFlagBits currentCapability : queueCapabilities)
    {
        if (find_if(infos.begin(), infos.end(), 
                [currentCapability](const QueueFamilyInfo& currentInfo)
                { return currentInfo.properties.queueFlags & currentCapability; }) != infos.end()) {
            continue;
        }

        auto pos = find_if(queueFamilyInfos.begin(), queueFamilyInfos.end(), 
            [currentCapability](const QueueFamilyInfo& currentInfo)
            { return currentInfo.properties.queueFlags & currentCapability; });
        RFX_CHECK_STATE(pos != queueFamilyInfos.end(), 
            "Required capability not found: " + to_string(currentCapability));
        infos.push_back(*pos);
    }

    return infos;
}

// ---------------------------------------------------------------------------------------------------------------------

int GraphicsContext::getDeviceGroupIndex(VkPhysicalDevice physicalDevice)
{
    for (size_t i = 0, count = deviceGroups.size(); i < count; ++i)
    {
        const VkPhysicalDeviceGroupProperties& currentGroup = deviceGroups[i];
        for (VkPhysicalDevice currentGroupDevice : currentGroup.physicalDevices)
        {
            if (currentGroupDevice == physicalDevice)
            {
                return static_cast<int>(i);
            }
        }
    }

    return -1;
}

// ---------------------------------------------------------------------------------------------------------------------
