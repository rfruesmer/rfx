#include "rfx/pch.h"
#include "rfx/graphics/effect/PointLightEffectFactory.h"
#include "rfx/graphics/effect/PointLightEffect.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

PointLightEffectFactory::PointLightEffectFactory()
    : EffectFactory("point_light") {}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Effect> PointLightEffectFactory::create(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass,
    unique_ptr<ShaderProgram>& shaderProgram,
    const vector<shared_ptr<Texture2D>>&) const
{
    return make_shared<PointLightEffect>(graphicsDevice, renderPass, shaderProgram);
}

// ---------------------------------------------------------------------------------------------------------------------
