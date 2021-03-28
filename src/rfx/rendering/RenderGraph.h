#pragma once

#include "rfx/scene/Model.h"
#include "rfx/scene/MaterialShader.h"
#include "rfx/rendering/ShaderNode.h"


namespace rfx {

/**
 *  TODO: support for multiple models
 */
class RenderGraph
{
public:
    explicit RenderGraph(GraphicsDevicePtr graphicsDevice);

    void add(
        const ModelPtr& model,
        const std::unordered_map<MaterialShaderPtr, std::vector<MaterialPtr>>& materialShaderMap);

    void record(
        const CommandBufferPtr& commandBuffer,
        VkDescriptorSet sceneDescriptorSet,
        VkRenderPass  renderPass,
        VkFramebuffer renderTarget);

private:
    void add(
        const MaterialShaderPtr& shader,
        const std::vector<MaterialPtr>& materials,
        const ModelPtr& model);

    void begin(
        const CommandBufferPtr& commandBuffer,
        VkRenderPass renderPass,
        VkFramebuffer renderTarget);
    void setViewportAndScissor(const CommandBufferPtr& commandBuffer) const;

    static void bindGeometryBuffers(
        const CommandBufferPtr& commandBuffer,
        const ModelPtr& model);

    static void recordShaderNodes(
        const std::vector<ShaderNode>& shaderNodes,
        VkDescriptorSet sceneDescriptorSet,
        const CommandBufferPtr& commandBuffer);

    static void end(const CommandBufferPtr& commandBuffer);


    GraphicsDevicePtr graphicsDevice;
    std::unordered_map<ModelPtr, std::vector<ShaderNode>> childNodeMap;
};

using RenderGraphPtr = std::shared_ptr<RenderGraph>;

} // namespace rfx
