#pragma once

#include "rfx/application/Window.h"
#include "rfx/application/DevTools.h"
#include "rfx/graphics/GraphicsContext.h"
#include "rfx/graphics/VertexShader.h"
#include "rfx/graphics/FragmentShader.h"


namespace rfx {

class Application : public std::enable_shared_from_this<Application>,
                    public WindowListener
{
public:
    Application();

    void run();

    void onResized(const Window& window, int width, int height) override;

protected:
    static std::filesystem::path getAssetsDirectory();

    virtual void initGraphics();
    virtual void update(int frameIndex);
    virtual void cleanup();
    virtual void cleanupSwapChain();
    virtual void recreateSwapChain();

    void createFrameBuffers();
    void createSyncObjects();

    std::shared_ptr<Window> window;
    std::unique_ptr<GraphicsContext> graphicsContext;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::vector<std::shared_ptr<CommandBuffer>> commandBuffers;

    VkPipeline graphicsPipeline = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

    bool devToolsEnabled = true;
    std::unique_ptr<DevTools> devTools;

private:
    void initialize();
    void initLogging();
    void initGlfw();
    void createGraphicsDevice();
    void createGraphicsContext();
    void createSwapChainAndDepthBuffer();
    void createWindow();
    void initDevTools();
    void drawDevTools(uint32_t frameIndex);
    void runMainLoop();
    void drawFrame();
    void destroySyncObjects();

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;

    size_t currentFrame = 0;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    bool windowResized = false;
    bool paused = false;
};

} // namespace rfx
