#pragma once

#include "rfx/graphics/effect/Effect.h"

namespace rfx
{

class DirectionalVertexLightingEffect : public Effect
{
public:
    static const std::string ID;

    explicit DirectionalVertexLightingEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        VkRenderPass renderPass,
        std::unique_ptr<ShaderProgram>& shaderProgram);

    const std::string& getId() const override;

private:
    void initDescriptorSetLayout();
    void initDescriptorSet();
};
    
}
