#pragma once

#include "rfx/graphics/effect/PointLightEffect.h"



namespace rfx
{

class SpotLightEffect : public PointLightEffect
{
public:
    static const std::string ID;

    explicit SpotLightEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        VkRenderPass renderPass,
        std::unique_ptr<ShaderProgram>& shaderProgram);

    const std::string& getId() const override;

    void updateFrom(const std::vector<std::shared_ptr<Light>>& lights) override;
};

}
