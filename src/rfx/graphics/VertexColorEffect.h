#pragma once

#include "rfx/graphics/Effect.h"
#include "rfx/graphics/GraphicsDevice.h"

namespace rfx
{

class VertexColorEffect : public Effect
{
public:
    explicit VertexColorEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        VkRenderPass renderPass,
        std::unique_ptr<ShaderProgram>& shaderProgram);

private:
    void initDescriptorSetLayout();
    void initDescriptorSet();
};
    
}
