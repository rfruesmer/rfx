#include "rfx/pch.h"
#include "test/texture-mapping/TexturedQuadTest.h"
#include "rfx/application/Texture2DLoader.h"
#include "rfx/application/ShaderLoader.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

static const VertexFormat VERTEX_FORMAT(VertexFormat::COORDINATES | VertexFormat::TEXCOORDS);

// ---------------------------------------------------------------------------------------------------------------------

TexturedQuadTest::TexturedQuadTest(handle_t instanceHandle)
    : TestApplication("assets/tests/textured-quad/application-config.json", instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::initialize()
{
    Application::initialize();

    initCommandPool();
    initRenderPass();
    initFrameBuffers();

    initEffects();
    initScene();
    
    initCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::initEffects()
{
    const ShaderLoader shaderLoader(graphicsDevice);
    shared_ptr<VertexShader> vertexShader = 
        shaderLoader.loadVertexShader("assets/common/shaders/texture.vert", "main", VERTEX_FORMAT);
    shared_ptr<FragmentShader> fragmentShader = 
        shaderLoader.loadFragmentShader("assets/common/shaders/texture.frag", "main");

    const Texture2DLoader textureLoader(graphicsDevice);
    shared_ptr<Texture2D> texture = textureLoader.load("assets/common/textures/lunarg_logo-256x256.png");

    std::unique_ptr<ShaderProgram> shaderProgram =
        make_unique<ShaderProgram>(vertexShader, fragmentShader);

    textureEffect = make_shared<Texture2DEffect>(graphicsDevice, renderPass, shaderProgram, texture);

    effects.push_back(textureEffect);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::initScene()
{
    createQuadMesh();
    initCamera();
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::createQuadMesh()
{
    const uint32_t vertexCount = 4;
    const uint32_t vertexBufferSize = vertexCount * VERTEX_FORMAT.getVertexSize();
    vector<float> vertexData = {
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };

    const shared_ptr<Buffer> stagingVertexBuffer = graphicsDevice->createBuffer(
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingVertexBuffer->load(vertexBufferSize,
        reinterpret_cast<const std::byte*>(vertexData.data()));
    stagingVertexBuffer->bind();

    shared_ptr<VertexBuffer> vertexBuffer = graphicsDevice->createVertexBuffer(vertexCount, VERTEX_FORMAT);
    vertexBuffer->bind();


    const uint32_t indexCount = 6;
    const uint32_t indexBufferSize = indexCount * sizeof(uint32_t);
    const vector<uint32_t> indexData = { 0, 1, 2, 2, 3, 0 };

    const shared_ptr<Buffer> stagingIndexBuffer = graphicsDevice->createBuffer(
        indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingIndexBuffer->load(indexBufferSize,
        reinterpret_cast<const std::byte*>(indexData.data()));
    stagingIndexBuffer->bind();

    shared_ptr<IndexBuffer> indexBuffer = graphicsDevice->createIndexBuffer(indexCount, VK_INDEX_TYPE_UINT32);
    indexBuffer->bind();

    const shared_ptr<CommandPool>& commandPool = graphicsDevice->getTempCommandPool();
    shared_ptr<CommandBuffer> commandBuffer = commandPool->allocateCommandBuffer();
    commandBuffer->begin();
    commandBuffer->copyBuffer(stagingVertexBuffer, vertexBuffer);
    commandBuffer->copyBuffer(stagingIndexBuffer, indexBuffer);
    commandBuffer->end();

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    VkFence fence = graphicsDevice->createFence(fenceCreateInfo);

    const shared_ptr<Queue>& queue = graphicsDevice->getGraphicsQueue();
    queue->submit(commandBuffer, fence);

    const VkResult result = graphicsDevice->waitForFences(1, &fence, true, DEFAULT_FENCE_TIMEOUT);
    RFX_CHECK_STATE(result == VK_SUCCESS, "failed to submit copy commands");

    graphicsDevice->destroyFence(fence);

    commandPool->freeCommandBuffer(commandBuffer);

    quadMesh = make_shared<Mesh>(graphicsDevice, vertexBuffer, indexBuffer, textureEffect);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::initCamera()
{
    camera = make_shared<Camera>();
    camera->setPosition(0.0F, 0.0F, 20.0F);
    camera->setLookAt(0.0F, 0.0F, 0.0F);
    camera->setUp(0.0F, 1.0F, 0.0F);
    camera->setProjection(45.0F, 1.0F, 0.1F, 10000.0F);

    onCameraModified();
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Camera>& TexturedQuadTest::getCamera() const
{
    return camera;
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::initCommandBuffers()
{
    drawCommandBuffers = commandPool->allocateCommandBuffers(graphicsDevice->getSwapChainBuffers().size());

    const VkExtent2D presentImageSize = graphicsDevice->getSwapChainProperties().imageSize;

    VkClearValue clearValues[2];
    clearValues[0].color = { 0.05F, 0.05F, 0.05F, 1.0F };
    clearValues[1].depthStencil.depth = 1.0F;
    clearValues[1].depthStencil.stencil = 0;

    VkViewport viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(presentImageSize.width);
    viewport.height = static_cast<float>(presentImageSize.height);
    viewport.minDepth = 0.0F;
    viewport.maxDepth = 1.0F;

    VkRect2D scissor;
    scissor.extent.width = presentImageSize.width;
    scissor.extent.height = presentImageSize.height;
    scissor.offset.x = 0;
    scissor.offset.y = 0;

    for (size_t i = 0, count = drawCommandBuffers.size(); i < count; ++i) {
        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.pNext = nullptr;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.framebuffer = frameBuffers[i];
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent = presentImageSize;
        renderPassBeginInfo.clearValueCount = 2;
        renderPassBeginInfo.pClearValues = clearValues;

        auto& commandBuffer = drawCommandBuffers[i];
        commandBuffer->begin();
        commandBuffer->beginRenderPass(renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        commandBuffer->setViewport(viewport);
        commandBuffer->setScissor(scissor);
            commandBuffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, textureEffect->getPipeline());
            commandBuffer->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, 
                textureEffect->getPipelineLayout(), textureEffect->getDescriptorSets());
            commandBuffer->bindVertexBuffers({ quadMesh->getVertexBuffer() });
            commandBuffer->bindIndexBuffer(quadMesh->getIndexBuffer());
            commandBuffer->drawIndexed(quadMesh->getIndexBuffer()->getIndexCount());
        commandBuffer->endRenderPass();
        commandBuffer->end();
    }

}

// ---------------------------------------------------------------------------------------------------------------------
