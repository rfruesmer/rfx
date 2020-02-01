#pragma once

#include "rfx/graphics/effect/Effect.h"

namespace rfx
{
    
class EffectFactory
{
public:
    explicit EffectFactory(const std::string& effectId);
    virtual ~EffectFactory() = default;

    [[nodiscard]] const std::string& getEffectId() const;

    virtual std::shared_ptr<Effect> create(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        VkRenderPass renderPass,
        std::unique_ptr<ShaderProgram>& shaderProgram,
        const std::vector<std::shared_ptr<Texture2D>>& textures) const = 0;

private:
    std::string effectId;
};

}
