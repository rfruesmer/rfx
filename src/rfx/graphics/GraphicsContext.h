#pragma once

#include "rfx/application/Window.h"
#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/graphics/SwapChain.h"

namespace rfx {

class GraphicsContext
{
public:
    explicit GraphicsContext(std::shared_ptr<Window> window); // TODO: move window argument to createGraphicsDevice
    ~GraphicsContext();

    void initialize();

    [[nodiscard]]
    VkInstance getInstance() const;

    [[nodiscard]]
    std::shared_ptr<GraphicsDevice> createGraphicsDevice(
        const VkPhysicalDeviceFeatures& features,
        const std::vector<std::string>& extensions,
        const std::vector<VkQueueFlagBits>& queueCapabilities);

private:
    static void dumpExtensions();
    void createVulkanInstance();
    void checkValidationSupport() const;
    void createValidationLoggerAdapter();
    void createPresentationSurface();
    void queryPhysicalDevices();
    static void queryProperties(VkPhysicalDevice physicalDevice, GraphicsDeviceDesc* deviceDesc);
    static void queryFeatures(VkPhysicalDevice physicalDevice, GraphicsDeviceDesc* deviceDesc);
    static void queryExtensions(VkPhysicalDevice physicalDevice, GraphicsDeviceDesc* deviceDesc);
    void queryQueueFamilies(VkPhysicalDevice physicalDevice, GraphicsDeviceDesc* deviceDesc);
    std::vector<QueueFamilyDesc> getQueueFamilyDescriptions(VkPhysicalDevice physicalDevice,
        const std::vector<VkQueueFamilyProperties>& queueFamilies) const;
    bool isPresentationSupported(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) const;
    void queryDeviceGroups();

    [[nodiscard]]
    VkPhysicalDevice findFirstMatchingPhysicalDevice(
        const VkPhysicalDeviceFeatures& features,
        const std::vector<std::string>& extensions,
        const std::vector<VkQueueFlagBits>& queueCapabilities) const;

    [[nodiscard]]
    bool isMatching(
        const GraphicsDeviceDesc& desc,
        const VkPhysicalDeviceFeatures& features,
        const std::vector<std::string>& extensions,
        const std::vector<VkQueueFlagBits>& queueCapabilities) const;

    [[nodiscard]]
    bool isDiscreteGPU(const GraphicsDeviceDesc& desc) const;

    [[nodiscard]]
    bool hasRequiredAPIVersion(const GraphicsDeviceDesc& deviceDesc) const;

    [[nodiscard]]
    bool hasRequiredFeatures(
        const GraphicsDeviceDesc& deviceDesc,
        const VkPhysicalDeviceFeatures& requiredFeatures) const;

    [[nodiscard]]
    bool hasRequiredExtensions(
        const GraphicsDeviceDesc& deviceDesc,
        const std::vector<std::string>& requiredExtensions) const;

    [[nodiscard]]
    bool hasRequiredQueueCapabilities(
        const GraphicsDeviceDesc& deviceDesc,
        const std::vector<VkQueueFlagBits>& requiredQueueCapabilities) const;

    [[nodiscard]]
    bool supportsSwapChain(const GraphicsDeviceDesc& desc) const;

    [[nodiscard]]
    static uint32_t getQueueFamilyIndex(
        const GraphicsDeviceDesc& deviceDesc,
        VkQueueFlags desiredCapabilities) ;

    [[nodiscard]]
    std::shared_ptr<GraphicsDevice> createLogicalDevice(
        VkPhysicalDevice physicalDevice,
        const VkPhysicalDeviceFeatures& features,
        const std::vector<std::string>& extensions,
        const std::vector<VkQueueFlagBits>& queueCapabilities) const;

    void selectQueueFamilies(
        VkPhysicalDevice physicalDevice,
        const std::vector<VkQueueFlagBits>& queueCapabilities,
        std::vector<QueueFamilyDesc>& outSelectedQueueFamilies,
        uint32_t& outGraphicsQueueFamilyIndex,
        uint32_t& outPresentQueueFamilyIndex) const;

    [[nodiscard]]
    int getDeviceGroupIndex(VkPhysicalDevice physicalDevice) const;

    static SurfaceDesc querySurfaceDesc(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

    const std::shared_ptr<Window> window;
    bool validationEnabled = false;
    std::vector<const char*> requiredLayers;
    VkInstance instance = nullptr;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    std::map<VkPhysicalDevice, GraphicsDeviceDesc> deviceDescs;
    std::vector<VkPhysicalDeviceGroupProperties> deviceGroups;
    VkSurfaceKHR presentSurface = VK_NULL_HANDLE;
};

} // namespace rfx
