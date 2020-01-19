#pragma once

#include "rfx/graphics/effect/Effect.h"
#include "rfx/graphics/GraphicsDevice.h"

namespace rfx
{

class VertexColorEffect : public Effect
{
public:
    static const std::string ID;

    explicit VertexColorEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        VkRenderPass renderPass,
        std::unique_ptr<ShaderProgram>& shaderProgram);

    const std::string& getId() const override;

private:
    void initDescriptorSetLayout();
    void initDescriptorSet();
};
    
}