#include "rfx/pch.h"
#include "TexturedCubeTest.h"
#include "rfx/graphics/Texture2DLoader.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

struct Vertex
{
    float x, y, z, w;   // Vertex Position
    float u, v;         // Texture format U,V
};

// ---------------------------------------------------------------------------------------------------------------------

static const Vertex vertices[] = {
    { -1.0f,-1.0f,-1.0f, 1.0f, 0.0f, 0.0f },  // -X side
    { -1.0f,-1.0f, 1.0f, 1.0f, 1.0f, 0.0f },
    { -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
    { -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
    { -1.0f, 1.0f,-1.0f, 1.0f, 0.0f, 1.0f },
    { -1.0f,-1.0f,-1.0f, 1.0f, 0.0f, 0.0f },

    //{ -1.0f,-1.0f,-1.0f, 1.0f, 0.0f, 1.0f },  // -Z side
    //{  1.0f, 1.0f,-1.0f, 1.0f, 1.0f, 0.0f },
    //{  1.0f,-1.0f,-1.0f, 1.0f, 1.0f, 1.0f },
    //{ -1.0f,-1.0f,-1.0f, 1.0f, 0.0f, 1.0f },
    //{ -1.0f, 1.0f,-1.0f, 1.0f, 0.0f, 0.0f },
    //{  1.0f, 1.0f,-1.0f, 1.0f, 1.0f, 0.0f },

    //{ -1.0f,-1.0f,-1.0f, 1.0f, 0.0f, 1.0f }, // -Y
    //{  1.0f,-1.0f,-1.0f, 1.0f, 0.0f, 0.0f },
    //{  1.0f,-1.0f, 1.0f, 1.0f, 1.0f, 0.0f },
    //{ -1.0f,-1.0f,-1.0f, 1.0f, 0.0f, 1.0f },
    //{  1.0f,-1.0f, 1.0f, 1.0f, 1.0f, 0.0f },
    //{ -1.0f,-1.0f, 1.0f, 1.0f, 1.0f, 1.0f },

    //{ -1.0f, 1.0f,-1.0f, 1.0f, 0.0f, 1.0f },  // +Y side
    //{ -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
    //{  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f },
    //{ -1.0f, 1.0f,-1.0f, 1.0f, 0.0f, 1.0f },
    //{  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f },
    //{  1.0f, 1.0f,-1.0f, 1.0f, 1.0f, 1.0f },

    { 1.0f, 1.0f,-1.0f, 1.0f, 1.0f, 1.0f },  // +X side
    { 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f },
    { 1.0f,-1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
    { 1.0f,-1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
    { 1.0f,-1.0f,-1.0f, 1.0f, 1.0f, 0.0f },
    { 1.0f, 1.0f,-1.0f, 1.0f, 1.0f, 1.0f },

    { -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f },  // +Z side
    { -1.0f,-1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
    {  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
    { -1.0f,-1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
    {  1.0f,-1.0f, 1.0f, 1.0f, 1.0f, 0.0f },
    {  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
};

// ---------------------------------------------------------------------------------------------------------------------

TexturedCubeTest::TexturedCubeTest(HINSTANCE instanceHandle)
    : TestApplication(instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t TexturedCubeTest::getVertexSize() const
{
    return sizeof(Vertex);
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t TexturedCubeTest::getVertexCount() const
{
    return 18;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::byte* TexturedCubeTest::getVertexData() const
{
    return reinterpret_cast<const std::byte*>(vertices);
}

// ---------------------------------------------------------------------------------------------------------------------

string TexturedCubeTest::getVertexShaderPath() const
{
    return "assets/shaders/texture.vert";
}

// ---------------------------------------------------------------------------------------------------------------------

string TexturedCubeTest::getFragmentShaderPath() const
{
    return "assets/shaders/texture.frag";
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedCubeTest::initPipelineLayout()
{
    VkDescriptorSetLayoutBinding layoutBindings[2] = {};
    layoutBindings[0].binding = 0;
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBindings[0].descriptorCount = 1;
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBindings[0].pImmutableSamplers = nullptr;

    layoutBindings[1].binding = 1;
    layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layoutBindings[1].descriptorCount = 1;
    layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    layoutBindings[1].pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount = 2;
    descriptorSetLayoutCreateInfo.pBindings = layoutBindings;

    descriptorSetLayout = graphicsDevice->createDescriptorSetLayout(descriptorSetLayoutCreateInfo);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

    pipelineLayout = graphicsDevice->createPipelineLayout(pipelineLayoutCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedCubeTest::initDescriptorPool()
{
    vector<VkDescriptorPoolSize> descriptorPoolSizes;
    descriptorPoolSizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1});
    descriptorPoolSizes.push_back({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1});

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.maxSets = 1;
    descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

    descriptorPool = graphicsDevice->createDescriptorPool(descriptorPoolCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedCubeTest::initDescriptorSet()
{
    loadTexture();

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = NUM_DESCRIPTOR_SETS;
    descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

    graphicsDevice->allocateDescriptorSets(descriptorSetAllocateInfo, descriptorSets);

    VkWriteDescriptorSet writes[2] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = nullptr;
    writes[0].dstSet = descriptorSets[0];
    writes[0].dstBinding = 0;
    writes[0].dstArrayElement = 0;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &uniformBuffer->getBufferInfo();

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].pNext = nullptr;
    writes[1].dstSet = descriptorSets[0];
    writes[1].dstBinding = 1;
    writes[1].dstArrayElement = 0;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].pImageInfo = &texture->getDescriptorImageInfo();

    graphicsDevice->updateDescriptorSets(2, writes);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedCubeTest::loadTexture()
{
    Texture2DLoader textureLoader(graphicsDevice);
    texture = textureLoader.load("assets/textures/lunarg_logo-256x256.png");
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedCubeTest::initVertexBuffer()
{
    TestApplication::initVertexBuffer();

    vertexInputAttributes[0].binding = 0;
    vertexInputAttributes[0].location = 0;
    vertexInputAttributes[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertexInputAttributes[0].offset = 0;

    vertexInputAttributes[1].binding = 0;
    vertexInputAttributes[1].location = 1;
    vertexInputAttributes[1].format = VK_FORMAT_R32G32_SFLOAT;
    vertexInputAttributes[1].offset = 16;
}

// ---------------------------------------------------------------------------------------------------------------------