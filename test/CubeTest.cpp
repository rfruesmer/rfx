#include "rfx/pch.h"
#include "test/CubeTest.h"
#include "rfx/scene/ModelLoader.h"
#include "rfx/graphics/VertexFormat.h"
#include "rfx/graphics/ShaderLoader.h"

using namespace rfx;
using namespace glm;
using namespace std;
using namespace std::filesystem;


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

    initScene();
    initCamera();

    initDescriptorSetLayout();
    initPipelineLayout();
    initPipeline();
    initDescriptorPool();
    initDescriptorSet();
    initCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeTest::initScene()
{
    createSceneGraphRootNode();
    loadModels();
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeTest::createSceneGraphRootNode()
{
    sceneGraph = make_unique<SceneNode>();
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeTest::loadModels()
{
    Json::Value jsonModels = configuration["scene"]["models"];

    for (const auto& jsonModel : jsonModels) {
        const shared_ptr<Mesh> mesh = loadModel(jsonModel);
        loadShaders(jsonModel, mesh);
        attachToSceneGraph(mesh);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Mesh> CubeTest::loadModel(const Json::Value& jsonModel) const
{
    const path path = current_path()
        / jsonModel["path"].asString();

    const VertexFormat vertexFormat(
        VertexFormat::COORDINATES | VertexFormat::COLORS);

    ModelLoader modelLoader(graphicsDevice);
    return modelLoader.load(path, vertexFormat);
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeTest::loadShaders(const Json::Value& jsonModel, const shared_ptr<Mesh>& mesh) const
{
    ShaderLoader shaderLoader(graphicsDevice);

    const path vertexShaderPath =
        current_path() / jsonModel["vertexShader"].asString();
    const VkPipelineShaderStageCreateInfo vertexShaderStage =
        shaderLoader.load(vertexShaderPath, VK_SHADER_STAGE_VERTEX_BIT, "main");
    mesh->setVertexShader(vertexShaderStage);

    const path fragmentShaderPath =
        current_path() / jsonModel["fragmentShader"].asString();
    const VkPipelineShaderStageCreateInfo fragmentShaderStage =
        shaderLoader.load(fragmentShaderPath, VK_SHADER_STAGE_FRAGMENT_BIT, "main");
    mesh->setFragmentShader(fragmentShaderStage);
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeTest::attachToSceneGraph(const shared_ptr<Mesh>& mesh) const
{
    sceneGraph->attach(mesh);
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeTest::initPipeline()
{
    RFX_CHECK_STATE(renderPass != nullptr, "Render pass must be setup before");

    VkDynamicState dynamicStateEnables[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = createDynamicState(2, dynamicStateEnables);
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = createInputAssemblyState();
    VkPipelineRasterizationStateCreateInfo rasterizationState = createRasterizationState();
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = createColorBlendAttachmentState();
    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = createColorBlendState(colorBlendAttachmentState);
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = createViewportState();
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = createDepthStencilState();
    VkPipelineMultisampleStateCreateInfo multiSampleStateCreateInfo = createMultiSampleState();

    const shared_ptr<Mesh>& mesh = sceneGraph->getMeshes().at(0);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = nullptr;
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.basePipelineHandle = nullptr;
    pipelineCreateInfo.basePipelineIndex = 0;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.pVertexInputState = &mesh->getVertexBuffer()->getInputState();
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    pipelineCreateInfo.pTessellationState = nullptr;
    pipelineCreateInfo.pMultisampleState = &multiSampleStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(mesh->getShaderStages().size());
    pipelineCreateInfo.pStages = mesh->getShaderStages().data();
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;

    pipeline = graphicsDevice->createGraphicsPipeline(pipelineCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeTest::initDescriptorPool()
{
    TestApplication::initDescriptorPool({
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}
    });
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeTest::initDescriptorSet()
{
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = NUM_DESCRIPTOR_SETS;
    descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

    graphicsDevice->allocateDescriptorSets(descriptorSetAllocateInfo, descriptorSets);

    VkWriteDescriptorSet writes = {};
    writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes.pNext = nullptr;
    writes.dstSet = descriptorSets[0];
    writes.descriptorCount = 1;
    writes.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes.pBufferInfo = &uniformBuffer->getBufferInfo();
    writes.dstArrayElement = 0;
    writes.dstBinding = 0;

    graphicsDevice->updateDescriptorSets(1, &writes);
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

    const shared_ptr<Mesh>& mesh = sceneGraph->getMeshes().at(0);

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
        commandBuffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        commandBuffer->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, descriptorSets);
        commandBuffer->bindVertexBuffers({ mesh->getVertexBuffer()  });
        commandBuffer->bindIndexBuffer(mesh->getIndexBuffer());
        commandBuffer->drawIndexed(mesh->getIndexBuffer()->getIndexCount());
        commandBuffer->endRenderPass();
        commandBuffer->end();
    }
}

// ---------------------------------------------------------------------------------------------------------------------