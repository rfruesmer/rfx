#include "rfx/pch.h"
#include "test/TexturedCubeTest.h"
#include "rfx/scene/ModelLoader.h"
#include "rfx/graphics/ShaderLoader.h"
#include "rfx/graphics/Texture2DLoader.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

TexturedCubeTest::TexturedCubeTest(handle_t instanceHandle)
    : CubeTest("assets/tests/textured-cube/application-config.json", instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedCubeTest::initialize()
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

void TexturedCubeTest::initScene()
{
    loadModel();
    loadShaders();
    loadTexture();
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedCubeTest::loadModel()
{
    Json::Value jsonModel = configuration["scene"]["models"][0];
    const path modelPath = current_path() / jsonModel["path"].asString();
    const VertexFormat vertexFormat(
        VertexFormat::COORDINATES | VertexFormat::TEXCOORDS);

    ModelLoader modelLoader(graphicsDevice);
    cube = modelLoader.load(modelPath, vertexFormat);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedCubeTest::loadShaders()
{
    Json::Value jsonModel = configuration["scene"]["models"][0];

    ShaderLoader shaderLoader(graphicsDevice);

    const path vertexShaderPath = 
        current_path() / jsonModel["vertexShader"].asString();
    const VkPipelineShaderStageCreateInfo vertexShaderStage =
        shaderLoader.load(vertexShaderPath, VK_SHADER_STAGE_VERTEX_BIT, "main");
    cube->setVertexShader(vertexShaderStage);

    const path fragmentShaderPath = 
        current_path() / jsonModel["fragmentShader"].asString();
    const VkPipelineShaderStageCreateInfo fragmentShaderStage =
        shaderLoader.load(fragmentShaderPath, VK_SHADER_STAGE_FRAGMENT_BIT, "main");
    cube->setFragmentShader(fragmentShaderStage);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedCubeTest::loadTexture()
{
    Json::Value jsonModel = configuration["scene"]["models"][0];
    const path texturePath = 
        current_path() / jsonModel["texture"].asString();
    
    Texture2DLoader textureLoader(graphicsDevice);
    texture = textureLoader.load(texturePath);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedCubeTest::initDescriptorSetLayout()
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
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedCubeTest::initDescriptorPool()
{
    TestApplication::initDescriptorPool({
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1}
    });
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedCubeTest::initDescriptorSet()
{
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
