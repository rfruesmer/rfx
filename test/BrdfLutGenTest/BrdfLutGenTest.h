#pragma once

#include "rfx/graphics/GraphicsContext.h"
#include "rfx/graphics/ComputeShader.h"

namespace rfx::test {

class BrdfLutGenTest
{
public:
    void run();

private:
    void initialize();
    static void initLogging();
    static void initGlfw();
    void createWindow();
    void createGraphicsContext();
    void createGraphicsDevice();

    void createComputeShader();
    std::filesystem::path getAssetsDirectory();
    void createComputeDescriptorPool();
    void createComputeDescriptorSetLayout();
    void createComputePipeline();
    void createComputeBuffers();
    void createComputeDescriptorSets();
    void createComputeCommandBuffer();

    void createLUT();
    void executeComputeShader();
    void fetchComputeShaderResult();
    void convertLUT();
    void saveLUT();

    void shutdown();

    WindowPtr window;
    std::unique_ptr<GraphicsContext> graphicsContext;
    GraphicsDevicePtr graphicsDevice;

    VkDescriptorPool computeDescriptorPool = nullptr;
    VkDescriptorSetLayout computeDescriptorSetLayout = nullptr;
    VkDescriptorSet computeDescriptorSet = nullptr;
    VkPipelineLayout computePipelineLayout = nullptr;
    VkPipeline computePipeline = nullptr;
    CommandBufferPtr computeCommandBuffer;
    ComputeShaderPtr computeShader;

    BufferPtr inputBuffer;
    BufferPtr outputBuffer;

    std::vector<float> lutData;
    gli::texture lutTexture;
};

} // namespace rfx::test