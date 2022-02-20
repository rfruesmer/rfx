#include "rfx/pch.h"
#include "rfx/scene/SkyBox.h"
#include "rfx/scene/SceneLoader.h"
#include "rfx/graphics/TextureLoader.h"
#include "rfx/graphics/ShaderLoader.h"
#include "rfx/graphics/PipelineUtil.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

SkyBox::SkyBox(
    GraphicsDevicePtr graphicsDevice,
    VkDescriptorPool descriptorPool)
        : graphicsDevice(move(graphicsDevice)),
          descriptorPool(descriptorPool) {}

// ---------------------------------------------------------------------------------------------------------------------

SkyBox::~SkyBox()
{
    VkDevice device = graphicsDevice->getLogicalDevice();

    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

    cleanupSwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------

void SkyBox::create(
    const path& modelPath,
    const path& cubeMapPath,
    const path& vertexShaderPath,
    const path& fragmentShaderPath,
    VkRenderPass renderPass)
{
    loadModel(modelPath);
    loadCubeMap(cubeMapPath);
    loadShaderProgram(vertexShaderPath, fragmentShaderPath);
    createUniformBuffer();
    createDescriptorSetLayout();
    createDescriptorSet();
    createPipeline(renderPass);
}

// ---------------------------------------------------------------------------------------------------------------------

void SkyBox::loadModel(const path& modelPath)
{
    SceneLoader sceneLoader(graphicsDevice);
    ScenePtr scene = sceneLoader.load(modelPath); // TODO: merge into scene vertex- & index-buffer

    model = scene->getModel(0);
}

// ---------------------------------------------------------------------------------------------------------------------

void SkyBox::loadCubeMap(const path& cubeMapPath)
{
    TextureLoader textureLoader(graphicsDevice);
    cubeMap = textureLoader.loadCubeMap(cubeMapPath);

    shaderData.mipCount = cubeMap->getImage()->getDesc().mipLevels;
}

// ---------------------------------------------------------------------------------------------------------------------

void SkyBox::loadShaderProgram(const path& vertexShaderPath, const path& fragmentShaderPath)
{
    shaderProgram =
        ShaderLoader(graphicsDevice).loadProgram(
            vertexShaderPath,
            fragmentShaderPath,
            model->getVertexBuffer()->getVertexFormat());
}

// ---------------------------------------------------------------------------------------------------------------------

void SkyBox::createUniformBuffer()
{
    uniformBuffer = graphicsDevice->createBuffer(
        sizeof(ShaderData),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    graphicsDevice->bind(uniformBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

void SkyBox::createDescriptorSetLayout()
{
    const vector<VkDescriptorSetLayoutBinding> descSetLayoutBindings {
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
        },
        {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        }
    };

    const VkDescriptorSetLayoutCreateInfo descSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast<uint32_t>(descSetLayoutBindings.size()),
        .pBindings = descSetLayoutBindings.data()
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice->getLogicalDevice(),
        &descSetLayoutCreateInfo,
        nullptr,
        &descriptorSetLayout));
}

// ---------------------------------------------------------------------------------------------------------------------

void SkyBox::createDescriptorSet()
{
    const VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayout
    };

    ThrowIfFailed(vkAllocateDescriptorSets(
        graphicsDevice->getLogicalDevice(),
        &allocInfo,
        &descriptorSet));

    vector<VkWriteDescriptorSet> writeDescriptorSets {
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &uniformBuffer->getDescriptorBufferInfo()
        },
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSet,
            .dstBinding = 1,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &cubeMap->getDescriptorImageInfo()
        }
    };

    vkUpdateDescriptorSets(
        graphicsDevice->getLogicalDevice(),
        writeDescriptorSets.size(),
        writeDescriptorSets.data(),
        0,
        nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void SkyBox::createPipeline(VkRenderPass renderPass)
{
    pipelineLayout = PipelineUtil::createPipelineLayout(
        graphicsDevice,
        {descriptorSetLayout});

    VkPipelineRasterizationStateCreateInfo rasterizationState =
        PipelineUtil::getDefaultRasterizationState();
    rasterizationState.cullMode = VK_CULL_MODE_FRONT_BIT;

    const VkPipelineColorBlendAttachmentState colorBlendAttachmentState =
        PipelineUtil::getDefaultColorBlendAttachmentState();

    VkPipelineDepthStencilStateCreateInfo depthStencilState =
        PipelineUtil::getDefaultDepthStencilState();
    depthStencilState.depthTestEnable = VK_FALSE;
    depthStencilState.depthWriteEnable = VK_FALSE;

    const vector<VkDynamicState> dynamicStates {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    pipeline = PipelineUtil::createGraphicsPipeline(
        graphicsDevice,
        pipelineLayout,
        PipelineUtil::getDefaultInputAssemblyState(),
        rasterizationState,
        PipelineUtil::getDefaultColorBlendState(&colorBlendAttachmentState),
        depthStencilState,
        PipelineUtil::getDefaultViewportState(),
        PipelineUtil::getDefaultMultisampleState(graphicsDevice->getMultiSampleCount()),
        PipelineUtil::getDynamicState(dynamicStates),
        shaderProgram,
        renderPass);
}

// ---------------------------------------------------------------------------------------------------------------------

void SkyBox::updateUniformBuffer(const CameraPtr& camera)
{
    glm::mat4 viewMatrix = camera->getViewMatrix();
    viewMatrix[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    shaderData.viewProjMatrix = camera->getProjectionMatrix() * viewMatrix;

    uniformBuffer->load(sizeof(ShaderData), &shaderData);
}

// ---------------------------------------------------------------------------------------------------------------------

void SkyBox::cleanupSwapChain()
{
    if (descriptorSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(graphicsDevice->getLogicalDevice(), descriptorSetLayout, nullptr);
        descriptorSetLayout = VK_NULL_HANDLE;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const ModelPtr& SkyBox::getModel() const
{
    return model;
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorSet SkyBox::getDescriptorSet() const
{
    return descriptorSet;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipeline SkyBox::getPipeline() const
{
    return pipeline;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineLayout SkyBox::getPipelineLayout() const
{
    return pipelineLayout;
}

// ---------------------------------------------------------------------------------------------------------------------

void SkyBox::setBlur(float factor)
{
    shaderData.blurFactor = factor;
}

// ---------------------------------------------------------------------------------------------------------------------

