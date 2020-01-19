#include "rfx/pch.h"
#include "test/CubeTest.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

CubeTest::CubeTest(handle_t instanceHandle)
    : TestApplication("assets/tests/cube/application-config.json", instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

CubeTest::CubeTest(const path& configurationPath, handle_t instanceHandle)
    : TestApplication(configurationPath, instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

void CubeTest::initialize()
{
    TestApplication::initialize();

    initCommandPool();
    initRenderPass();
    initFrameBuffers();

    initEffects();
    initScene();

    initCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeTest::initScene()
{
    createSceneGraphRootNode();
    loadModels();
    initCamera();
}

// ---------------------------------------------------------------------------------------------------------------------


void CubeTest::initCommandBuffers()
{
    drawCommandBuffers = commandPool->allocateCommandBuffers(graphicsDevice->getSwapChainBuffers().size());

    const VkExtent2D presentImageSize = graphicsDevice->getSwapChainProperties().imageSize;

    VkClearValue clearValues[2] = {};
    clearValues[0].color = { { 0.05F, 0.05F, 0.05F, 1.0F } };
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

    const shared_ptr<Effect>& currentEffect = effects[0];

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
            commandBuffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, currentEffect->getPipeline());
            commandBuffer->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, 
                currentEffect->getPipelineLayout(), currentEffect->getDescriptorSets());
            drawNode(sceneGraph, commandBuffer);
        commandBuffer->endRenderPass();
        commandBuffer->end();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeTest::drawNode(const unique_ptr<SceneNode>& sceneNode,
    const shared_ptr<CommandBuffer>& commandBuffer)
{
    for (const auto& mesh : sceneNode->getMeshes()) {
        commandBuffer->bindVertexBuffers({ mesh->getVertexBuffer() });
        commandBuffer->bindIndexBuffer(mesh->getIndexBuffer());
        commandBuffer->drawIndexed(mesh->getIndexBuffer()->getIndexCount());
    }

    for (const auto& childNode : sceneNode->getChildNodes()) {
        drawNode(childNode, commandBuffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
