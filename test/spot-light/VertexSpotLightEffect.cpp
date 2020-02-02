#include "rfx/pch.h"
#include "test/spot-light/VertexSpotLightEffect.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

const string VertexSpotLightEffect::ID = "vertex_spot_light";

// ---------------------------------------------------------------------------------------------------------------------

VertexSpotLightEffect::VertexSpotLightEffect(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass,
    std::unique_ptr<ShaderProgram>& shaderProgram)
        : VertexPointLightEffect(graphicsDevice, renderPass, shaderProgram) {}

// ---------------------------------------------------------------------------------------------------------------------

const string& VertexSpotLightEffect::getId() const
{
    return ID;
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexSpotLightEffect::updateFrom(const std::vector<std::shared_ptr<Light>>& lights)
{
    RFX_CHECK_ARGUMENT(!lights.empty());
    RFX_CHECK_ARGUMENT(lights[0]->getType() == LightType::SPOT);

    uniformData.lightData = lights[0]->getData();
}

// ---------------------------------------------------------------------------------------------------------------------

