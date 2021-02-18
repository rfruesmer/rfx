#include "rfx/pch.h"
#include "TexturedQuad.h"
#include "rfx/application/Texture2DLoader.h"
#include "rfx/application/ShaderLoader.h"

using namespace rfx;
using namespace rfx::test;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuad::buildScene()
{
    createTexture();

    const path assetsPath = getAssetsPath();
    const path vertexShaderPath = assetsPath / "textured-triangle.vert";
    const path fragmentShaderPath = assetsPath / "textured-triangle.frag";

    const ShaderLoader shaderLoader(graphicsDevice);
    vertexShader = shaderLoader.loadVertexShader(
        vertexShaderPath,
        "main",
        VertexFormat(VertexFormat::COORDINATES | VertexFormat::COLORS | VertexFormat::TEXCOORDS));
    fragmentShader = shaderLoader.loadFragmentShader(fragmentShaderPath, "main");

    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuad::createTexture()
{
    Texture2DLoader textureLoader(graphicsDevice);
    texture = textureLoader.load(getAssetsPath() / "metalplate01_rgba.ktx");
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuad::createVertexBuffer()
{
    struct Vertex {
        glm::vec3 pos;
        glm::vec4 color;
        glm::vec2 texCoord;
    };

    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };

    const VertexFormat vertexFormat(VertexFormat::COORDINATES | VertexFormat::COLORS | VertexFormat::TEXCOORDS);
    const VkDeviceSize bufferSize = vertices.size() * vertexFormat.getVertexSize();
    shared_ptr<Buffer> stagingBuffer = graphicsDevice->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* data = nullptr;
    graphicsDevice->bind(stagingBuffer);
    graphicsDevice->map(stagingBuffer, &data);
    memcpy(data, vertices.data(), stagingBuffer->getSize());
    graphicsDevice->unmap(stagingBuffer);

    vertexBuffer = graphicsDevice->createVertexBuffer(vertices.size(), vertexFormat);
    graphicsDevice->bind(vertexBuffer);

    VkCommandPool graphicsCommandPool = graphicsDevice->getGraphicsCommandPool();
    shared_ptr<CommandBuffer> commandBuffer = graphicsDevice->createCommandBuffer(graphicsCommandPool);
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    commandBuffer->copyBuffer(stagingBuffer, vertexBuffer);
    commandBuffer->end();

    const shared_ptr<Queue>& graphicsQueue = graphicsDevice->getGraphicsQueue();
    graphicsQueue->submit(commandBuffer);
    graphicsQueue->waitIdle();

    graphicsDevice->destroyCommandBuffer(commandBuffer, graphicsCommandPool);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuad::cleanup()
{
    texture.reset();
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuad::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uniformBufferLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = nullptr // Optional
    };

    VkDescriptorSetLayoutBinding samplerLayoutBinding {
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = nullptr
    };

    std::array<VkDescriptorSetLayoutBinding, 2> layoutBindings {
        uniformBufferLayoutBinding,
        samplerLayoutBinding
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast<uint32_t>(layoutBindings.size()),
        .pBindings = layoutBindings.data()
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice->getLogicalDevice(),
        &layoutInfo,
        nullptr,
        &descriptorSetLayout));
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuad::createDescriptorPool()
{
    const SwapChainDesc& swapChainDesc = graphicsDevice->getSwapChain()->getDesc();

    std::array<VkDescriptorPoolSize, 2> poolSizes {};
    poolSizes[0] = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = static_cast<uint32_t>(swapChainDesc.bufferCount)
    };

    poolSizes[1] = {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = static_cast<uint32_t>(swapChainDesc.bufferCount)
    };

    VkDescriptorPoolCreateInfo poolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = swapChainDesc.bufferCount,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data()
    };

    ThrowIfFailed(vkCreateDescriptorPool(
        graphicsDevice->getLogicalDevice(),
        &poolCreateInfo,
        nullptr,
        &descriptorPool));
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuad::createDescriptorSets()
{
    const SwapChainDesc& swapChainDesc = graphicsDevice->getSwapChain()->getDesc();

    std::vector<VkDescriptorSetLayout> layouts(swapChainDesc.bufferCount, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = swapChainDesc.bufferCount,
        .pSetLayouts = layouts.data()
    };

    descriptorSets.resize(swapChainDesc.bufferCount);

    ThrowIfFailed(vkAllocateDescriptorSets(
        graphicsDevice->getLogicalDevice(),
        &allocInfo,
        descriptorSets.data()));

    VkDescriptorBufferInfo bufferInfo {
        .offset = 0,
        .range = sizeof(UniformBufferObject)
    };

    VkDescriptorImageInfo imageInfo {
        .sampler = texture->getSampler(),
        .imageView = texture->getImageView(),
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };

    std::array<VkWriteDescriptorSet, 2> descriptorWrites {};
    descriptorWrites[0] = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = &bufferInfo
    };

    descriptorWrites[1] = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstBinding = 1,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &imageInfo
    };

    for (uint32_t i = 0; i < swapChainDesc.bufferCount; ++i) {
        bufferInfo.buffer = uniformBuffers[i]->getHandle();
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[1].dstSet = descriptorSets[i];

        vkUpdateDescriptorSets(
            graphicsDevice->getLogicalDevice(),
            static_cast<uint32_t>(descriptorWrites.size()),
            descriptorWrites.data(),
            0,
            nullptr);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
