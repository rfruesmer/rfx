#pragma once

#include "rfx/graphics/Effect.h"

namespace rfx
{

class Texture2DEffect : public Effect
{
public:
    static const std::string ID;

    explicit Texture2DEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        VkRenderPass renderPass,
        std::unique_ptr<ShaderProgram>& shaderProgram,
        const std::shared_ptr<Texture2D>& texture);

private:
    void initDescriptorSetLayout();
    void initDescriptorSet();

    std::shared_ptr<Texture2D> texture;
};
    
}
