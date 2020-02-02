#pragma once

#include "rfx/graphics/effect/EffectFactory.h"


namespace rfx
{

class VertexPointLightEffectFactory : public EffectFactory
{
public:
    explicit VertexPointLightEffectFactory();

    std::shared_ptr<Effect> create(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        VkRenderPass renderPass,
        std::unique_ptr<ShaderProgram>& shaderProgram,
        const std::vector<std::shared_ptr<Texture2D>>& textures) const override;
};

} // namespace rfx
