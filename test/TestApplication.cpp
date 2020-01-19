#include "rfx/pch.h"

#include "test/TestApplication.h"
#include "rfx/scene/ModelLoader.h"
#include "rfx/scene/ModelDefinition.h"
#include "rfx/scene/ModelDefinitionDeserializer.h"
#include "rfx/graphics/ShaderLoader.h"
#include "rfx/graphics/Texture2DLoader.h"
#include "rfx/graphics/Texture2DEffect.h"

using namespace rfx;
using namespace glm;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

TestApplication::TestApplication(const path& configurationPath, handle_t instanceHandle)
#ifdef _WINDOWS
    : Win32Application(configurationPath, instanceHandle)
#else
static_assert(false, "not implemented yet");
#endif // _WINDOWS
{
    glslang::InitializeProcess();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createSceneGraphRootNode()
{
    sceneGraph = make_unique<SceneNode>();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::loadModels()
{
    Json::Value jsonModelDefinitions = configuration["scene"]["models"];

    for (const auto& jsonModelDefinition : jsonModelDefinitions) {
        ModelDefinition modelDefinition = deserialize(jsonModelDefinition);
        const shared_ptr<Mesh> mesh = loadModel(modelDefinition);
        loadShaders(mesh, modelDefinition);
        loadTexture(mesh, modelDefinition);
        attachToSceneGraph(mesh, modelDefinition);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ModelDefinition TestApplication::deserialize(const Json::Value& jsonModelDefinition) const
{
    const ModelDefinitionDeserializer deserializer;
    return deserializer.deserialize(jsonModelDefinition);
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Mesh> TestApplication::loadModel(const ModelDefinition& modelDefinition) const
{
    RFX_NOT_IMPLEMENTED();

    ModelLoader modelLoader(graphicsDevice);
    return modelLoader.load(modelDefinition.modelPath, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::loadShaders(const shared_ptr<Mesh>& mesh, 
    const ModelDefinition& modelDefinition) const
{
    RFX_NOT_IMPLEMENTED();

    ShaderLoader shaderLoader(graphicsDevice);

    shared_ptr<VertexShader> vertexShader = 
        shaderLoader.loadVertexShader(modelDefinition.vertexShaderPath, "main", modelDefinition.vertexFormat);

    shared_ptr<FragmentShader> fragmentShader =
        shaderLoader.loadFragmentShader(modelDefinition.fragmentShaderPath, "main");

    shared_ptr<ShaderProgram> shaderProgram =
        make_shared<ShaderProgram>(vertexShader, fragmentShader);

    // TODO
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::loadTexture(const shared_ptr<Mesh>& mesh, 
    const ModelDefinition& modelDefinition)
{
    RFX_NOT_IMPLEMENTED();

    //const path& texturePath = modelDefinition.texturePath; 
    //if (texturePath.empty()) {
    //    return;
    //}

    //const Texture2DLoader textureLoader(graphicsDevice);
    //const shared_ptr<Texture2D> texture = textureLoader.load(texturePath);

    //shared_ptr<Texture2DEffect> textureEffect = make_shared<Texture2DEffect>(
    //    graphicsDevice,
    //    descriptorPool,
    //    renderPass,
    //    texture);
    //effects.push_back(textureEffect);

}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::attachToSceneGraph(const shared_ptr<Mesh>& mesh, 
    const ModelDefinition& modelDefinition) const
{
    unique_ptr<SceneNode> sceneNode = make_unique<SceneNode>();

    const Transform& transform = modelDefinition.transform;
    if (!transform.isIdentity()) {
        Transform& localTransform = sceneNode->getLocalTransform();
        localTransform.setTranslation(transform.getTranslation());
        localTransform.setScale(transform.getScale());
        localTransform.setRotation(transform.getRotation());
        localTransform.update();

        sceneNode->updateWorldTransform();
    }

    sceneNode->attach(mesh);

    sceneGraph->attach(sceneNode);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initCamera()
{
    modelMatrix = mat4(1.0F);
    cameraPosition = vec3(0.0F, 0.0F, 10.0F);
    cameraLookAt = vec3(0.0F);
    cameraUp = vec3(0.0F, 1.0F, 0.0F);
    projectionMatrix = perspective(radians(45.0F), 1.0F, 0.1F, 100.0F);

    updateViewProjectionMatrix();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::updateViewProjectionMatrix()
{
    viewMatrix = lookAt(cameraPosition, cameraLookAt, cameraUp);
    viewProjMatrix = projectionMatrix * viewMatrix;

    onViewProjectionMatrixUpdated();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initCommandPool()
{
    commandPool = graphicsDevice->createCommandPool(
        graphicsDevice->getDeviceInfo().graphicsQueueFamilyIndex);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initRenderPass()
{
    VkAttachmentDescription attachments[2];
    attachments[0].format = graphicsDevice->getSwapChainFormat();
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachments[0].flags = 0;

    attachments[1].format = GraphicsDevice::DEPTHBUFFER_FORMAT;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments[1].flags = 0;

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorReference;
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = &depthReference;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;

    VkSubpassDependency subpassDependency = {};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependency.dependencyFlags = 0;

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = nullptr;
    renderPassCreateInfo.attachmentCount = 2;
    renderPassCreateInfo.pAttachments = attachments;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    renderPass = graphicsDevice->createRenderPass(renderPassCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initFrameBuffers()
{
    const GraphicsDeviceInfo& deviceInfo = graphicsDevice->getDeviceInfo();

    VkImageView attachments[2];
    attachments[1] = graphicsDevice->getDepthBuffer().imageView;

    VkFramebufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.pNext = nullptr;
    frameBufferCreateInfo.renderPass = renderPass;
    frameBufferCreateInfo.attachmentCount = 2;
    frameBufferCreateInfo.pAttachments = attachments;
    frameBufferCreateInfo.width = graphicsDevice->getSwapChainProperties().imageSize.width;
    frameBufferCreateInfo.height = graphicsDevice->getSwapChainProperties().imageSize.height;
    frameBufferCreateInfo.layers = 1;

    const vector<SwapChainBuffer>& swapChainBuffers = graphicsDevice->getSwapChainBuffers();
    frameBuffers.resize(swapChainBuffers.size());

    for (size_t i = 0, count = swapChainBuffers.size(); i < count; ++i) {
        attachments[0] = swapChainBuffers[i].imageView;
        frameBuffers[i] = graphicsDevice->createFrameBuffer(frameBufferCreateInfo);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::update()
{
    static const float MOVE_DELTA = 0.005F;

    Win32Application::update();

    bool cameraNeedsUpdate = false;

    if (keyboard->isKeyDown(Keyboard::KEY_W)) {
        cameraPosition.z -= MOVE_DELTA;
        cameraLookAt.z -= MOVE_DELTA;
        cameraNeedsUpdate = true;
    }

    if (keyboard->isKeyDown(Keyboard::KEY_S)) {
        cameraPosition.z += MOVE_DELTA;
        cameraLookAt.z += MOVE_DELTA;
        cameraNeedsUpdate = true;
    }

    if (keyboard->isKeyDown(Keyboard::KEY_A)) {
        cameraPosition.x -= MOVE_DELTA;
        cameraLookAt.x -= MOVE_DELTA;
        cameraNeedsUpdate = true;
    }

    if (keyboard->isKeyDown(Keyboard::KEY_D)) {
        cameraPosition.x += MOVE_DELTA;
        cameraLookAt.x += MOVE_DELTA;
        cameraNeedsUpdate = true;
    }

    if (keyboard->isKeyDown(Keyboard::KEY_UP)) {
        cameraPosition.y -= MOVE_DELTA;
        cameraLookAt.y -= MOVE_DELTA;
        cameraNeedsUpdate = true;
    }

    if (keyboard->isKeyDown(Keyboard::KEY_DOWN)) {
        cameraPosition.y += MOVE_DELTA;
        cameraLookAt.y += MOVE_DELTA;
        cameraNeedsUpdate = true;
    }

    if (keyboard->isKeyDown(Keyboard::KEY_ESCAPE)) {
        PostQuitMessage(0);
    }

    if (cameraNeedsUpdate) {
        updateViewProjectionMatrix();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::draw()
{
    VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
    imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    imageAcquiredSemaphoreCreateInfo.pNext = nullptr;
    imageAcquiredSemaphoreCreateInfo.flags = 0;

    VkSemaphore imageAcquiredSemaphore = graphicsDevice->createSemaphore(imageAcquiredSemaphoreCreateInfo);
    uint32_t nextImageIndex = UINT32_MAX;
    VkResult result = graphicsDevice->acquireNextSwapChainImage(
        UINT64_MAX, imageAcquiredSemaphore, nullptr, nextImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || windowResized) {
        windowResized = false;
        recreateSwapChain();
        graphicsDevice->destroySemaphore(imageAcquiredSemaphore);
        return;
    }
    else if (result != VK_SUCCESS) {
        RFX_CHECK_STATE(false, "Failed to acquire next swap chain image");
    }

    VkFenceCreateInfo fenceCreateInfo;
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = 0;
    auto drawFence = graphicsDevice->createFence(fenceCreateInfo);

    VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkCommandBuffer vkCommandBuffers[] = { drawCommandBuffers[nextImageIndex]->getHandle() };

    VkSubmitInfo submitInfo = {};
    submitInfo.pNext = nullptr;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAcquiredSemaphore;
    submitInfo.pWaitDstStageMask = &pipelineStageFlags;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = vkCommandBuffers;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    graphicsDevice->getGraphicsQueue()->submit(1, &submitInfo, drawFence);

    VkPresentInfoKHR presentInfo;
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &graphicsDevice->getSwapChain();
    presentInfo.pImageIndices = &nextImageIndex;
    presentInfo.pWaitSemaphores = nullptr;
    presentInfo.waitSemaphoreCount = 0;
    presentInfo.pResults = nullptr;

    do {
        result = graphicsDevice->waitForFences(1, &drawFence, true, DEFAULT_FENCE_TIMEOUT);
    } while (result == VK_TIMEOUT);
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to execute command buffer");

    graphicsDevice->getPresentQueue()->present(presentInfo);

    graphicsDevice->destroySemaphore(imageAcquiredSemaphore);
    graphicsDevice->destroyFence(drawFence);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::shutdown()
{
    destroyFrameBuffers();
    freeCommandBuffers();
    destroyRenderPass();
    destroyDescriptors();
    destroyBuffers();

    glslang::FinalizeProcess();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::destroyFrameBuffers()
{
    for (auto& frameBuffer : frameBuffers) {
        graphicsDevice->destroyFrameBuffer(frameBuffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::freeCommandBuffers() const
{
    commandPool->freeAllCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::onViewProjectionMatrixUpdated()
{
    for (const auto& effect : effects) {
        effect->setViewProjMatrix(viewProjMatrix);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::destroyRenderPass()
{
    graphicsDevice->destroyRenderPass(renderPass);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::destroyDescriptors()
{
    graphicsDevice->destroyDescriptorSetLayout(descriptorSetLayout);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::destroyBuffers() const
{
    // no-op
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::recreateSwapChain()
{
    graphicsDevice->waitIdle();

    destroyFrameBuffers();
    freeCommandBuffers();
//    destroyPipeline();
    destroyRenderPass();
    destroySwapChainAndDepthBuffer();

    createSwapChainAndDepthBuffer();
    initRenderPass();
//    initPipeline();
    initFrameBuffers();
    initCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::destroySwapChainAndDepthBuffer() const
{
    graphicsDevice->destroyDepthBuffer();
    graphicsDevice->destroySwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createSwapChainAndDepthBuffer() const
{
    graphicsDevice->createSwapChain();
    graphicsDevice->createDepthBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------
