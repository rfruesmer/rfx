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
    RenderGraph(
        GraphicsDevicePtr graphicsDevice,
        VkDescriptorSet sceneDescriptorSet);

    void add(
        const ModelPtr& model,
        const std::unordered_map<MaterialShaderPtr, std::vector<MaterialPtr>>& materialShaderMap);

    void add(RenderGraphNodePtr userDefinedNode);

    void record(
        const CommandBufferPtr& commandBuffer,
        VkRenderPass renderPass,
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
        const CommandBufferPtr& commandBuffer);

    static void end(const CommandBufferPtr& commandBuffer);


    GraphicsDevicePtr graphicsDevice;
    VkDescriptorSet sceneDescriptorSet = VK_NULL_HANDLE;
    std::unordered_map<ModelPtr, std::vector<ShaderNode>> childNodeMap;
    std::vector<RenderGraphNodePtr> userDefinedNodes;
};

using RenderGraphPtr = std::shared_ptr<RenderGraph>;

} // namespace rfx
