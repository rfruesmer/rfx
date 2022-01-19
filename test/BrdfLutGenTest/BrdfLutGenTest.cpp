#include "rfx/pch.h"
#include "BrdfLutGenTest.h"
#include "rfx/graphics/PipelineUtil.h"
#include "rfx/graphics/ShaderLoader.h"
#include "rfx/common/Logger.h"

int shutdown();

using namespace rfx;
using namespace rfx::test;
using namespace std;
using namespace filesystem;

static const uint32_t brdfWidth = 256;
static const uint32_t brdfHeight = 256;
static const uint32_t outputBufferSize = 2 * sizeof(float) * brdfWidth * brdfHeight;

// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    try {
        auto theApp = make_shared<BrdfLutGenTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

static void onGlfwError(int, const char* description) {
    RFX_LOG_ERROR << description;
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::run()
{
    initialize();

    createLUT();
    convertLUT();
    saveLUT();

    shutdown();
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::initialize()
{
    glslang::InitializeProcess();

    initLogging();
    initGlfw();
    createWindow();
    createGraphicsContext();
    createGraphicsDevice();

    createComputeShader();
    createComputeDescriptorPool();
    createComputeDescriptorSetLayout();
    createComputePipeline();
    createComputeBuffers();
    createComputeDescriptorSets();
    createComputeCommandBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::initLogging()
{
#ifdef _DEBUG
    Logger::setLogLevel(LogLevel::DEBUG);
#endif // _DEBUG
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::initGlfw()
{
    if (!glfwInit()) {
        RFX_THROW("GLFW initialization failed");
    }

    glfwSetErrorCallback(onGlfwError);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::createWindow()
{
    window = make_unique<Window>();
    window->create("rfx", 1, 1);
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::createGraphicsContext()
{
    graphicsContext = make_unique<GraphicsContext>(window);
    graphicsContext->initialize();
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::createGraphicsDevice()
{
    VkPhysicalDeviceFeatures features {
        .geometryShader = VK_TRUE
    };

    graphicsDevice = graphicsContext->createGraphicsDevice(
        features,
        { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_MAINTENANCE1_EXTENSION_NAME},
        { VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT });
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::createComputeShader()
{
    const path shaderPath = getAssetsDirectory() / "shaders/pbr_gltf/brdf_lut.comp";

    ShaderLoader shaderLoader(graphicsDevice);
    computeShader = shaderLoader.loadComputeShader(shaderPath, "main");
}

// ---------------------------------------------------------------------------------------------------------------------

path BrdfLutGenTest::getAssetsDirectory()
{
    filesystem::path assetsPath = filesystem::current_path();

//#ifdef _DEBUG
    assetsPath = assetsPath.parent_path();
//#endif

    return assetsPath / "assets";
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::createComputeDescriptorPool()
{
    VkDevice device = graphicsDevice->getLogicalDevice();

    VkDescriptorPoolSize descriptorPoolSize = {
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        2
    };

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        nullptr,
        0,
        1,
        1,
        &descriptorPoolSize
    };

    ThrowIfFailed(vkCreateDescriptorPool(
        device,
        &descriptorPoolCreateInfo,
        nullptr,
        &computeDescriptorPool));
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::createComputeDescriptorSetLayout()
{
    VkDevice device = graphicsDevice->getLogicalDevice();

    VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[2] = {
        {
            0,
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            1,
            VK_SHADER_STAGE_COMPUTE_BIT,
            nullptr
        },
        {
            1,
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            1,
            VK_SHADER_STAGE_COMPUTE_BIT,
            nullptr
        }
    };

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        nullptr,
        0,
        2,
        descriptorSetLayoutBindings
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        device,
        &descriptorSetLayoutCreateInfo,
        nullptr,
        &computeDescriptorSetLayout));
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::createComputeBuffers()
{
    inputBuffer = graphicsDevice->createSharedBuffer(
        sizeof(float),
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    graphicsDevice->bind(inputBuffer);

    outputBuffer = graphicsDevice->createSharedBuffer(
        outputBufferSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    graphicsDevice->bind(outputBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::createComputeDescriptorSets()
{
    VkDevice device = graphicsDevice->getLogicalDevice();

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        nullptr,
        computeDescriptorPool,
        1,
        &computeDescriptorSetLayout
    };

    ThrowIfFailed(vkAllocateDescriptorSets(
        device,
        &descriptorSetAllocateInfo,
        &computeDescriptorSet));

    VkDescriptorBufferInfo inBufferInfo = {
        inputBuffer->getHandle(),
        0,
        VK_WHOLE_SIZE };

    VkDescriptorBufferInfo outBufferInfo = {
        outputBuffer->getHandle(),
        0,
        VK_WHOLE_SIZE
    };

    VkWriteDescriptorSet writeDescriptorSet[2] = {
        {
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            nullptr,
            computeDescriptorSet,
            0,
            0,
            1,
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            nullptr,
            &inBufferInfo,
            nullptr
        },
        {
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            nullptr,
            computeDescriptorSet,
            1,
            0,
            1,
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            nullptr,
            &outBufferInfo,
            nullptr
        }
    };

    vkUpdateDescriptorSets(
        device,
        2,
        writeDescriptorSet,
        0,
        nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::createComputePipeline()
{
    const VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &computeDescriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };

    ThrowIfFailed(vkCreatePipelineLayout(
        graphicsDevice->getLogicalDevice(),
        &pipelineLayoutInfo,
        nullptr,
        &computePipelineLayout));

    computePipeline = PipelineUtil::createComputePipeline(
        graphicsDevice,
        computePipelineLayout,
        computeShader);
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::createComputeCommandBuffer()
{
    computeCommandBuffer = graphicsDevice->createCommandBuffer(
        graphicsDevice->getComputeCommandPool());
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::createLUT()
{
    executeComputeShader();
    fetchComputeShaderResult();
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::executeComputeShader()
{
    computeCommandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    computeCommandBuffer->bindPipeline(VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
    computeCommandBuffer->bindDescriptorSet(
        VK_PIPELINE_BIND_POINT_COMPUTE,
        computePipelineLayout,
        0,
        computeDescriptorSet);
    computeCommandBuffer->dispatch(brdfWidth, brdfHeight, 1);

    VkMemoryBarrier memoryBarrier = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_HOST_READ_BIT
    };

    computeCommandBuffer->pipelineBarrier(
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_PIPELINE_STAGE_HOST_BIT,
        memoryBarrier);

    computeCommandBuffer->end();

    const QueuePtr& computeQueue = graphicsDevice->getComputeQueue();
    computeQueue->submit(computeCommandBuffer);
    computeQueue->waitIdle();
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::fetchComputeShaderResult()
{
    lutData.resize(outputBufferSize);
    outputBuffer->save(outputBufferSize, lutData.data());
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::convertLUT()
{
    lutTexture = gli::texture2d(gli::FORMAT_RG16_SFLOAT_PACK16, gli::extent2d(brdfWidth, brdfHeight), 1);

    for (uint32_t y = 0; y < brdfHeight; y++)
    {
        for (uint32_t x = 0; x < brdfWidth; x++)
        {
            const int ofs = y * brdfWidth + x;
            const gli::vec2 value(lutData[ofs * 2 + 0], lutData[ofs * 2 + 1]);
            const gli::texture::extent_type uv = { x, y, 0 };
            lutTexture.store<glm::uint32>(uv, 0, 0, 0, gli::packHalf2x16(value));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::saveLUT()
{
    const path destPath = getAssetsDirectory() / "shaders/pbr_gltf/brdfLUT.ktx";

    gli::save_ktx(lutTexture, destPath.string());
}

// ---------------------------------------------------------------------------------------------------------------------

void BrdfLutGenTest::shutdown()
{
    computeCommandBuffer.reset();

    vkDestroyPipelineLayout(
        graphicsDevice->getLogicalDevice(),
        computePipelineLayout,
        nullptr);

    vkDestroyPipeline(
        graphicsDevice->getLogicalDevice(),
        computePipeline,
        nullptr);

    vkDestroyDescriptorSetLayout(
        graphicsDevice->getLogicalDevice(),
        computeDescriptorSetLayout,
        nullptr);

    vkDestroyDescriptorPool(
        graphicsDevice->getLogicalDevice(),
        computeDescriptorPool,
        nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------
