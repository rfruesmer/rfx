#include "rfx/pch.h"
#include "test/basic/TriangleTest.h"
#include "rfx/application/ShaderLoader.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

static const VertexFormat VERTEX_FORMAT(VertexFormat::COORDINATES | VertexFormat::COLORS);

// ---------------------------------------------------------------------------------------------------------------------

TriangleTest::TriangleTest(handle_t instanceHandle)
    : TestApplication("assets/tests/triangle/application-config.json", instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

void TriangleTest::initialize()
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

void TriangleTest::initEffects()
{
    const ShaderLoader shaderLoader(graphicsDevice);
    shared_ptr<VertexShader> vertexShader =
        shaderLoader.loadVertexShader("assets/common/shaders/color.vert", "main", VERTEX_FORMAT);
    shared_ptr<FragmentShader> fragmentShader =
        shaderLoader.loadFragmentShader("assets/common/shaders/color.frag", "main");

    std::unique_ptr<ShaderProgram> shaderProgram = 
        make_unique<ShaderProgram>(vertexShader, fragmentShader);

    vertexColorEffect = make_shared<VertexColorEffect>(graphicsDevice, renderPass, shaderProgram);

    effects.push_back(vertexColorEffect);
}

// ---------------------------------------------------------------------------------------------------------------------

void TriangleTest::initScene()
{
    createTriangleMesh();
    initCamera();
}

// ---------------------------------------------------------------------------------------------------------------------

void TriangleTest::createTriangleMesh()
{
    const uint32_t vertexCount = 3;
    const uint32_t vertexBufferSize = vertexCount * VERTEX_FORMAT.getVertexSize();
    vector<float> vertexData = {
        1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
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


    const uint32_t indexCount = 3;
    const uint32_t indexBufferSize = indexCount * sizeof(uint32_t);
    const vector<uint32_t> indexData = { 0, 1, 2 };

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

    triangleMesh = make_shared<Mesh>(graphicsDevice, vertexBuffer, indexBuffer, vertexColorEffect);
}

// ---------------------------------------------------------------------------------------------------------------------

void TriangleTest::initCamera()
{
    camera = make_shared<Camera>();
    camera->setPosition(0.0F, 0.0F, 20.0F);
    camera->setLookAt(0.0F, 0.0F, 0.0F);
    camera->setUp(0.0F, 1.0F, 0.0F);
    camera->setProjection(45.0F, 1.0F, 0.1F, 10000.0F);

    onCameraModified();
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Camera>& TriangleTest::getCamera() const
{
    return camera;
}

// ---------------------------------------------------------------------------------------------------------------------

void TriangleTest::initCommandBuffers()
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
            commandBuffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, vertexColorEffect->getPipeline());
            commandBuffer->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, 
                vertexColorEffect->getPipelineLayout(), vertexColorEffect->getDescriptorSets());
            commandBuffer->bindVertexBuffers({ triangleMesh->getVertexBuffer() });
            commandBuffer->bindIndexBuffer(triangleMesh->getIndexBuffer());
            commandBuffer->drawIndexed(triangleMesh->getIndexBuffer()->getIndexCount());
        commandBuffer->endRenderPass();
        commandBuffer->end();
    }
}

// ---------------------------------------------------------------------------------------------------------------------
