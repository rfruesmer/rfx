#include "rfx/pch.h"
#include "rfx/application/Application.h"
#include "rfx/common/Logger.h"
#include "rfx/common/to.h"

using namespace rfx;
using namespace std;
using namespace std::chrono;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

static const int MAX_FRAMES_IN_FLIGHT = 2;

// ---------------------------------------------------------------------------------------------------------------------

static void onGlfwError(int, const char* description) {
    RFX_LOG_ERROR << description;
}

// ---------------------------------------------------------------------------------------------------------------------

Application::Application()
{
    glslang::InitializeProcess();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::run()
{
    initialize();
    runMainLoop();
    cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::initialize()
{
    initLogging();
    initGlfw();
    createWindow();
    initGraphics();
    initDevTools();
    showWindow();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::initLogging()
{
#ifdef _DEBUG
    Logger::setLogLevel(LogLevel::DEBUG);
#endif // _DEBUG
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::initGlfw()
{
    if (!glfwInit()) {
        RFX_THROW("GLFW initialization failed");
    }

    glfwSetErrorCallback(onGlfwError);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::createWindow()
{
    window_ = make_unique<Window>();
    window_->create("rfx", 1200, 675);
    window_->addListener(shared_from_this());

    glfwSetInputMode(window_->getGlfwWindow(), GLFW_STICKY_KEYS, GLFW_TRUE);
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::initGraphics()
{
    createGraphicsContext();
    createGraphicsDevice();
    createSwapChain();
    createMultiSamplingBuffer();
    createDepthBuffer();
    createSyncObjects();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::createGraphicsContext()
{
    graphicsContext = make_unique<GraphicsContext>(window_);
    graphicsContext->initialize();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::createGraphicsDevice()
{
    VkPhysicalDeviceFeatures features {
        .geometryShader = VK_TRUE,
        .fillModeNonSolid = VK_TRUE,
        .samplerAnisotropy = VK_TRUE
    };

    graphicsDevice = graphicsContext->createGraphicsDevice(
        features,
        { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_MAINTENANCE1_EXTENSION_NAME},
        { VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT });
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::createSwapChain()
{
    graphicsDevice->createSwapChain(
        window_->getClientWidth(),
        window_->getClientHeight());
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::createDepthBuffer()
{
    graphicsDevice->createDepthBuffer(GraphicsDevice::DEFAULT_DEPTHBUFFER_FORMAT);
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::createMultiSamplingBuffer()
{
    graphicsDevice->createMultiSamplingBuffer(graphicsDevice->getDesc().maxSampleCount);
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::initDevTools()
{
    if (devToolsEnabled) {
        devTools = make_unique<DevTools>(
            window_,
            graphicsContext,
            graphicsDevice);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::showWindow()
{
    window_->show();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::drawDevTools()
{
    if (devToolsEnabled) {
        devTools->beginDraw(currentImageIndex, lastFPS);
        updateDevTools();
        devTools->endDraw();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

path Application::getAssetsDirectory()
{
    filesystem::path assetsPath = filesystem::current_path();

//#ifdef _DEBUG
    assetsPath = assetsPath.parent_path();
//#endif

    return assetsPath / "assets";
}

// ---------------------------------------------------------------------------------------------------------------------

path Application::getShadersDirectory()
{
    return getAssetsDirectory() / "shaders";
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::createFrameBuffers()
{
    graphicsDevice->getSwapChain()->createFrameBuffers(
        renderPass,
        graphicsDevice->getDepthBuffer(),
        graphicsDevice->getMultiSampleImageView());
}

// ---------------------------------------------------------------------------------------------------------------------


void Application::createSyncObjects()
{
    const SwapChainDesc& swapChainDesc = graphicsDevice->getSwapChain()->getDesc();
    
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    fencesInFlight.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(swapChainDesc.bufferCount, VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        ThrowIfFailed(vkCreateSemaphore(
            graphicsDevice->getLogicalDevice(),
            &semaphoreCreateInfo,
            nullptr,
            &imageAvailableSemaphores[i]));

        ThrowIfFailed(vkCreateSemaphore(
            graphicsDevice->getLogicalDevice(),
            &semaphoreCreateInfo,
            nullptr,
            &renderFinishedSemaphores[i]));

        ThrowIfFailed(vkCreateFence(
            graphicsDevice->getLogicalDevice(),
            &fenceInfo,
            nullptr,
            &fencesInFlight[i]));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::runMainLoop()
{
    beginMainLoop();

    while (isRunning())
    {
        beginFrame();

        glfwPollEvents();
        if (paused) {
            continue;
        }

        if (acquireNextImage()) {
            updateFrameDeltaTime();
            update(deltaTime);
            drawDevTools();
            submitAndPresent();
        }

        endFrame();
    }

    endMainLoop();
}

// ---------------------------------------------------------------------------------------------------------------------

bool Application::isRunning() const
{
    return !glfwWindowShouldClose(window_->getGlfwWindow());
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::beginMainLoop()
{
    frameDeltaStopWatch.start();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::beginFrame()
{
    frameStopWatch.start();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::endFrame()
{
    StopWatch::TimePoint stopTime = frameStopWatch.stop();
    float fpsTimer = static_cast<float>(duration<double, milli>(stopTime - lastFPSUpdateTimePoint).count());
    if (fpsTimer >= 1000.0f) {
        lastFPS = static_cast<uint32_t>((float) frameCounter * (1000.0f / fpsTimer));
        frameCounter = 0;
        lastFPSUpdateTimePoint = stopTime;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool Application::acquireNextImage()
{
    vkWaitForFences(graphicsDevice->getLogicalDevice(), 1, &fencesInFlight[currentFrame], VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(
        graphicsDevice->getLogicalDevice(),
        graphicsDevice->getSwapChain()->getHandle(),
        UINT64_MAX,
        imageAvailableSemaphores[currentFrame],
        VK_NULL_HANDLE,
        &currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || windowResized) {
        windowResized = false;
        recreateSwapChain();
        return false;
    }
    RFX_CHECK_STATE(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swap chain image");

    if (imagesInFlight[currentImageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(graphicsDevice->getLogicalDevice(), 1, &imagesInFlight[currentImageIndex], VK_TRUE, UINT64_MAX);
    }

    imagesInFlight[currentImageIndex] = fencesInFlight[currentFrame];
    vkResetFences(graphicsDevice->getLogicalDevice(), 1, &fencesInFlight[currentFrame]);

    frameCounter++;

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::submitAndPresent()
{
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame] };
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    vector<VkCommandBuffer> submitCommandBuffers {
        commandBuffers[currentImageIndex]->getHandle()
    };
    if (devToolsEnabled) {
        submitCommandBuffers.push_back(devTools->getCommandBuffer(currentImageIndex));
    }

    VkSubmitInfo submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = static_cast<uint32_t>(submitCommandBuffers.size()),
        .pCommandBuffers = submitCommandBuffers.data(),
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores
    };
    graphicsDevice->getGraphicsQueue()->submit(submitInfo, fencesInFlight[currentFrame]);

    VkSwapchainKHR swapChains[] = { graphicsDevice->getSwapChain()->getHandle() };

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapChains,
        .pImageIndices = &currentImageIndex,
        .pResults = nullptr // Optional
    };

    VkResult result = graphicsDevice->getPresentationQueue()->present(presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || windowResized) {
        windowResized = false;
        recreateSwapChain();
        return;
    }
    RFX_CHECK_STATE(VK_SUCCEEDED(result), "Failed to present swap chain image");

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::cleanup()
{
    devTools.reset();

    cleanupSwapChain();
    destroySyncObjects();

    graphicsDevice.reset();
    graphicsContext.reset();

    glfwDestroyWindow(window_->getGlfwWindow());
    window_ = nullptr;

    glfwTerminate();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::destroySyncObjects()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(graphicsDevice->getLogicalDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(graphicsDevice->getLogicalDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(graphicsDevice->getLogicalDevice(), fencesInFlight[i], nullptr);
    }

    renderFinishedSemaphores.clear();
    imageAvailableSemaphores.clear();
    fencesInFlight.clear();
    imagesInFlight.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::cleanupSwapChain()
{
    VkDevice vkDevice = graphicsDevice->getLogicalDevice();

    vkDestroyDescriptorPool(vkDevice, descriptorPool, nullptr);

    auto commandBufferHandles = commandBuffers
            | views::transform([](const shared_ptr<CommandBuffer>& commandBuffer)
                { return commandBuffer->getHandle(); })
            | to<vector>();

    vkFreeCommandBuffers(
        vkDevice,
        graphicsDevice->getGraphicsCommandPool(),
        static_cast<uint32_t>(commandBufferHandles.size()),
        commandBufferHandles.data());

    vkDestroyPipeline(vkDevice, defaultPipeline, nullptr);
    vkDestroyPipelineLayout(vkDevice, pipelineLayout, nullptr);
    vkDestroyRenderPass(vkDevice, renderPass, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::recreateSwapChain()
{
    graphicsDevice->waitIdle();

    devTools.reset();
    destroySyncObjects();
    cleanupSwapChain();
    createSyncObjects();
    createSwapChain();
    createMultiSamplingBuffer();
    createDepthBuffer();
    initDevTools();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::onResized(const Window&, int width, int height)
{
    windowResized = true;

    paused = (width == 0 || height == 0);
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::update(float deltaTime)
{
    // do nothing
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::endMainLoop() const
{
    vkDeviceWaitIdle(graphicsDevice->getLogicalDevice());
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::updateFrameDeltaTime()
{
    frameDeltaStopWatch.stop();

    deltaTime = duration<float, std::milli>(frameDeltaStopWatch.getElapsedTime()).count();

    frameDeltaStopWatch.start();
}

// ---------------------------------------------------------------------------------------------------------------------
