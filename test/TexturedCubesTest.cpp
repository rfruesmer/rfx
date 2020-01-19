#include "rfx/pch.h"
#include "test/TexturedCubesTest.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace filesystem;

/**
// ---------------------------------------------------------------------------------------------------------------------

TexturedCubesTest::TexturedCubesTest(handle_t instanceHandle)
    : CubeTest("assets/tests/textured-cubes/application-config.json", instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedCubesTest::initialize()
{
    TestApplication::initialize();

    initCommandPool();
    initRenderPass();
    initFrameBuffers();

    initEffects();
    initScene();
    
    initDescriptorSetLayout();
    initPipelineLayout();
    initPipeline();
    initDescriptorPool();
    initCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedCubesTest::initEffects()
{
    // no-op
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedCubesTest::initDescriptorSetLayout()
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

void TexturedCubesTest::initDescriptorPool()
{
    TestApplication::initDescriptorPool({
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1}
    });
}

// ---------------------------------------------------------------------------------------------------------------------
*/