#pragma once

#include "test/point-light/VertexPointLightEffect.h"



namespace rfx
{

class VertexSpotLightEffect : public VertexPointLightEffect
{
public:
    static const std::string ID;

    explicit VertexSpotLightEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        VkRenderPass renderPass,
        std::unique_ptr<ShaderProgram>& shaderProgram);

    const std::string& getId() const override;

    void updateFrom(const std::vector<std::shared_ptr<Light>>& lights) override;
};

}
