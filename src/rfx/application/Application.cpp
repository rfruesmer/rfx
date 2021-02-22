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
    window = make_unique<Window>();
    window->create("rfx", 800, 450);
    window->addListener(shared_from_this());
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::initGraphics()
{
    createGraphicsContext();
    createGraphicsDevice();
    createSwapChainAndDepthBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::createGraphicsContext()
{
    graphicsContext = make_unique<GraphicsContext>(window);
    graphicsContext->initialize();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::createGraphicsDevice()
{
    VkPhysicalDeviceFeatures features {
        .geometryShader = VK_TRUE,
        .samplerAnisotropy = VK_TRUE
    };

    graphicsDevice = graphicsContext->createGraphicsDevice(
        features,
        { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_MAINTENANCE1_EXTENSION_NAME},
        { VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT });
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::createSwapChainAndDepthBuffer()
{
    const int width = window->getClientWidth();
    const int height = window->getClientHeight();

    graphicsDevice->createSwapChain(width, height);
    graphicsDevice->createDepthBuffer(GraphicsDevice::DEFAULT_DEPTHBUFFER_FORMAT);
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::initDevTools()
{
    if (devToolsEnabled) {
        devTools = make_unique<DevTools>(
            window,
            graphicsContext,
            graphicsDevice);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::drawDevTools()
{
    if (devToolsEnabled) {
        devTools->draw(currentImageIndex, lastFPS);
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

void Application::createFrameBuffers()
{
    graphicsDevice->getSwapChain()->createFrameBuffers(renderPass, graphicsDevice->getDepthBuffer());
}

// ---------------------------------------------------------------------------------------------------------------------


void Application::createSyncObjects()
{
    const SwapChainDesc& swapChainDesc = graphicsDevice->getSwapChain()->getDesc();
    
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
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
            &inFlightFences[i]));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::runMainLoop()
{
    while (!glfwWindowShouldClose(window->getGlfwWindow()))
    {
        beginFrame();

        glfwPollEvents();
        if (paused) {
            continue;
        }

        if (acquireNextImage()) {
            update();
            drawDevTools();
            submitAndPresent();
        }

        endFrame();
    }

    vkDeviceWaitIdle(graphicsDevice->getLogicalDevice());
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::beginFrame()
{
    stopWatch.start();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::endFrame()
{
    StopWatch::TimePoint stopTime = stopWatch.stop();
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
    vkWaitForFences(graphicsDevice->getLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

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

    imagesInFlight[currentImageIndex] = inFlightFences[currentFrame];
    vkResetFences(graphicsDevice->getLogicalDevice(), 1, &inFlightFences[currentFrame]);

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
    graphicsDevice->getGraphicsQueue()->submit(submitInfo, inFlightFences[currentFrame]);

    VkSwapchainKHR swapChains[] = {graphicsDevice->getSwapChain()->getHandle() };

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

    glfwDestroyWindow(window->getGlfwWindow());
    window = nullptr;

    glfwTerminate();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::destroySyncObjects()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(graphicsDevice->getLogicalDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(graphicsDevice->getLogicalDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(graphicsDevice->getLogicalDevice(), inFlightFences[i], nullptr);
    }

    renderFinishedSemaphores.clear();
    imageAvailableSemaphores.clear();
    inFlightFences.clear();
    imagesInFlight.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::cleanupSwapChain()
{
    VkDevice vkDevice = graphicsDevice->getLogicalDevice();

    auto commandBufferHandles = commandBuffers
            | views::transform([](const shared_ptr<CommandBuffer>& commandBuffer)
                { return commandBuffer->getHandle(); })
            | to<vector>();

    vkFreeCommandBuffers(
        vkDevice,
        graphicsDevice->getGraphicsCommandPool(),
        static_cast<uint32_t>(commandBufferHandles.size()),
        commandBufferHandles.data());

    vkDestroyPipeline(vkDevice, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(vkDevice, pipelineLayout, nullptr);
    vkDestroyRenderPass(vkDevice, renderPass, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::recreateSwapChain()
{
    graphicsDevice->waitIdle();

    devTools.reset();

    destroySyncObjects();
    createSyncObjects();

    cleanupSwapChain();
    createSwapChainAndDepthBuffer();
    initDevTools();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::onResized(const Window&, int width, int height)
{
    windowResized = true;

    paused = (width == 0 || height == 0);
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::update()
{
    // do nothing
}


// ---------------------------------------------------------------------------------------------------------------------
