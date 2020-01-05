#pragma once

#include "rfx/graphics/GraphicsDeviceInfo.h"
#include "rfx/graphics/QueueFamilyInfo.h"
#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/application/Window.h"

namespace rfx
{

class GraphicsContext
{
public:
    ~GraphicsContext();
    void dispose();

    void initialize(const std::string& applicationName, const std::shared_ptr<Window>& window);
    void initialize(const std::string& applicationName, const std::shared_ptr<Window>& window,
        const std::vector<std::string>& instanceExtensions);

    std::shared_ptr<GraphicsDevice> createGraphicsDevice(
        const VkPhysicalDeviceFeatures& features,
        const std::vector<std::string>& extensions,
        const std::vector<VkQueueFlagBits>& queueCapabilities);
    
private:

#ifdef _DEBUG
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objectType,
        uint64_t object,
        size_t location,
        int32_t messageCode,
        const char* pLayerPrefix,
        const char* pMessage,
        void* pUserData);
#endif // _DEBUG

    void loadVulkanLibrary();
    void loadVulkanLoaderFunctions();
    void loadGlobalFunctions();
    void queryInstanceExtensions();
    void checkThatRequestedInstanceExtensionsAreAvailable(const std::vector<std::string>& requestedExtensions);
    void createVulkanInstance(const std::string& applicationName,
        const std::vector<std::string>& requestedExtensions);
    void loadInstanceFunctions();
    void createPresentationSurface(const std::shared_ptr<Window>& window);
    void queryPhysicalDevices(const std::shared_ptr<Window>& window);
    void queryExtensions(VkPhysicalDevice device, GraphicsDeviceInfo& outDeviceInfo) const;
    void queryFeatures(VkPhysicalDevice device, GraphicsDeviceInfo& outDeviceInfo) const;
    void queryProperties(VkPhysicalDevice device, GraphicsDeviceInfo& outDeviceInfo) const;
    void queryQueueFamilies(VkPhysicalDevice device, GraphicsDeviceInfo& outDeviceInfo) const;
    std::vector<QueueFamilyInfo> getQueueFamilyInfos(VkPhysicalDevice device, 
        const std::vector<VkQueueFamilyProperties>& queueFamilies) const;
    bool isPresentationSupported(VkPhysicalDevice device, uint32_t queueFamilyIndex) const;
    void queryPresentModes(VkPhysicalDevice device, 
        const std::shared_ptr<Window>& window, 
        GraphicsDeviceInfo& outDeviceInfo) const;
    void queryPresentationSurfaceCapabilities(VkPhysicalDevice device, 
        GraphicsDeviceInfo& outDeviceInfo) const;
    void queryPresentationSurfaceFormats(VkPhysicalDevice device, GraphicsDeviceInfo& outDeviceInfo) const;
    void querySwapChainImageSize(VkPhysicalDevice device,
        const std::shared_ptr<Window>& window,
        GraphicsDeviceInfo& outDeviceInfo) const;
    void queryDeviceGroups();

    VkPhysicalDevice findFirstMatchingPhysicalDevice(
        const VkPhysicalDeviceFeatures& features, 
        const std::vector<std::string>& extensions,
        const std::vector<VkQueueFlagBits>& queueCapabilities);
    bool isMatching(const GraphicsDeviceInfo& deviceInfo, 
                    const VkPhysicalDeviceFeatures& features, 
                    const std::vector<std::string>& extensions,
                    const std::vector<VkQueueFlagBits>& queueCapabilities) const;
    bool hasRequiredAPIVersion(const GraphicsDeviceInfo& deviceInfo) const;
    bool hasFeatures(const GraphicsDeviceInfo& deviceInfo,
        const VkPhysicalDeviceFeatures& expectedFeatures) const;
    bool hasExtensions(const GraphicsDeviceInfo& deviceInfo,
        const std::vector<std::string>& expectedExtensions) const;
    bool hasQueueCapabilities(const GraphicsDeviceInfo& deviceInfo,
        const std::vector<VkQueueFlagBits>& expectedQueueCapabilities) const;
    size_t getQueueFamilyIndex(const GraphicsDeviceInfo& deviceInfo, 
        VkQueueFlags desiredCapabilities) const;
    std::shared_ptr<GraphicsDevice> createLogicalDevice(VkPhysicalDevice physicalDevice,
                                                        const VkPhysicalDeviceFeatures& features,
                                                        const std::vector<std::string>& extensions, 
                                                        const std::vector<VkQueueFlagBits>& queueCapabilities);
    std::vector<QueueFamilyInfo> pickQueueFamilies(VkPhysicalDevice dDevice,
        const std::vector<VkQueueFlagBits>& queueCapabilities);
    int getDeviceGroupIndex(VkPhysicalDevice physicalDevice);

    handle_t vkLibrary = nullptr;
    VkInstance vkInstance = nullptr;
    VkSurfaceKHR presentationSurface = nullptr;
    std::unordered_map<std::string, VkExtensionProperties> instanceExtensions;
    std::map<VkPhysicalDevice, GraphicsDeviceInfo> deviceInfos;
    std::vector<VkPhysicalDeviceGroupProperties> deviceGroups;

    DECLARE_VULKAN_FUNCTION(vkGetInstanceProcAddr);

    DECLARE_VULKAN_FUNCTION(vkEnumerateInstanceVersion);
    DECLARE_VULKAN_FUNCTION(vkEnumerateInstanceExtensionProperties);
    DECLARE_VULKAN_FUNCTION(vkEnumerateInstanceLayerProperties);
    DECLARE_VULKAN_FUNCTION(vkCreateInstance);

#ifdef _DEBUG
    DECLARE_VULKAN_FUNCTION(vkCreateDebugReportCallbackEXT);
    DECLARE_VULKAN_FUNCTION(vkDebugReportMessageEXT);
    DECLARE_VULKAN_FUNCTION(vkDestroyDebugReportCallbackEXT);
#endif // DEBUG

    DECLARE_VULKAN_FUNCTION(vkEnumeratePhysicalDevices);
    DECLARE_VULKAN_FUNCTION(vkEnumerateDeviceExtensionProperties);
    DECLARE_VULKAN_FUNCTION(vkEnumeratePhysicalDeviceGroups);
    DECLARE_VULKAN_FUNCTION(vkGetPhysicalDeviceFeatures);
    DECLARE_VULKAN_FUNCTION(vkGetPhysicalDeviceProperties);
    DECLARE_VULKAN_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);
    DECLARE_VULKAN_FUNCTION(vkGetPhysicalDeviceMemoryProperties);
    DECLARE_VULKAN_FUNCTION(vkGetPhysicalDeviceFormatProperties);
    DECLARE_VULKAN_FUNCTION(vkCreateDevice);
    DECLARE_VULKAN_FUNCTION(vkGetDeviceProcAddr);
    DECLARE_VULKAN_FUNCTION(vkDestroyInstance);

    DECLARE_VULKAN_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);
    DECLARE_VULKAN_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    DECLARE_VULKAN_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
    DECLARE_VULKAN_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
    DECLARE_VULKAN_FUNCTION(vkDestroySurfaceKHR);

#ifdef VK_USE_PLATFORM_WIN32_KHR
    DECLARE_VULKAN_FUNCTION(vkCreateWin32SurfaceKHR);
#elif defined VK_USE_PLATFORM_XCB_KHR
    DECLARE_VULKAN_FUNCTION(vkCreateXcbSurfaceKHR);
#elif defined VK_USE_PLATFORM_XLIB_KHR
    DECLARE_VULKAN_FUNCTION(vkCreateXlibSurfaceKHR);
#endif

};

} // namespace rfx
