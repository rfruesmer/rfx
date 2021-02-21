#include "rfx/pch.h"
#include "rfx/application/Application.h"
#include "rfx/common/Logger.h"
#include "rfx/common/to.h"

using namespace rfx;
using namespace std;
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
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window->getGlfwWindow(), &width, &height);

    graphicsDevice->createSwapChain(width, height);
    graphicsDevice->createDepthBuffer(GraphicsDevice::DEFAULT_DEPTHBUFFER_FORMAT);
}

// ---------------------------------------------------------------------------------------------------------------------

path Application::getAssetsPath()
{
    filesystem::path assetsPath = filesystem::current_path();

#ifdef _DEBUG
    assetsPath = assetsPath.parent_path();
#endif

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
    while (!glfwWindowShouldClose(window->getGlfwWindow())) {
        glfwPollEvents();

        if (!paused) {
            drawFrame();
        }
    }

    vkDeviceWaitIdle(graphicsDevice->getLogicalDevice());
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::drawFrame()
{
    vkWaitForFences(graphicsDevice->getLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(
        graphicsDevice->getLogicalDevice(),
        graphicsDevice->getSwapChain()->getHandle(),
        UINT64_MAX,
        imageAvailableSemaphores[currentFrame],
        VK_NULL_HANDLE,
        &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || windowResized) {
        windowResized = false;
        recreateSwapChain();
        return;
    }
    RFX_CHECK_STATE(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swap chain image");

    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(graphicsDevice->getLogicalDevice(), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    imagesInFlight[imageIndex] = inFlightFences[currentFrame];
    vkResetFences(graphicsDevice->getLogicalDevice(), 1, &inFlightFences[currentFrame]);

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    update(imageIndex);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffers[imageIndex]->getHandle(),
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores
    };
    graphicsDevice->getGraphicsQueue()->submit(submitInfo, inFlightFences[currentFrame]);

    VkSwapchainKHR swapChains[] = { graphicsDevice->getSwapChain()->getHandle() };

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapChains,
        .pImageIndices = &imageIndex,
        .pResults = nullptr // Optional
    };

    result = graphicsDevice->getPresentationQueue()->present(presentInfo);
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

    destroySyncObjects();
    createSyncObjects();

    cleanupSwapChain();
    createSwapChainAndDepthBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::onResized(const Window&, int width, int height)
{
    windowResized = true;

    paused = (width == 0 || height == 0);
}

// ---------------------------------------------------------------------------------------------------------------------
