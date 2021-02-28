#pragma once

#include <rfx/common/StopWatch.h>
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

protected:
    static std::filesystem::path getAssetsDirectory();

    void onResized(const Window& window, int width, int height) override;

    virtual void initGraphics();
    virtual void beginMainLoop();
    virtual void update(float deltaTime);
    virtual void cleanup();
    virtual void cleanupSwapChain();
    virtual void recreateSwapChain();

    void createFrameBuffers();
    void createSyncObjects();

    std::shared_ptr<Window> window_;
    std::unique_ptr<GraphicsContext> graphicsContext;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::vector<std::shared_ptr<CommandBuffer>> commandBuffers;

    VkPipeline defaultPipeline = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    uint32_t currentImageIndex = 0;

    bool devToolsEnabled = true;
    std::unique_ptr<DevTools> devTools;

private:
    void initialize();
    void initLogging();
    void initGlfw();
    void createGraphicsDevice();
    void createGraphicsContext();
    void createSwapChain();
    void createDepthBuffer();
    void createMultiSamplingBuffer();
    void createWindow();
    void initDevTools();
    void runMainLoop();

    bool isRunning() const;
    void beginFrame();
    bool acquireNextImage();
    void updateFrameDeltaTime();
    void drawDevTools();
    virtual void updateDevTools() {};
    void submitAndPresent();
    void endFrame();
    void endMainLoop() const;

    void destroySyncObjects();

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;

    size_t currentFrame = 0;
    std::vector<VkFence> fencesInFlight;
    std::vector<VkFence> imagesInFlight;
    bool windowResized = false;
    bool paused = false;

    StopWatch frameStopWatch;
    StopWatch frameDeltaStopWatch;
    float deltaTime = 0.0f;

    uint32_t frameCounter = 0;
    uint32_t lastFPS = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastFPSUpdateTimePoint;
};

} // namespace rfx
