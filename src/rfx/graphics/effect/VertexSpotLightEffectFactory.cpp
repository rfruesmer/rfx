#include "rfx/pch.h"
#include "rfx/graphics/effect/VertexSpotLightEffectFactory.h"
#include "rfx/graphics/effect/VertexSpotLightEffect.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

VertexSpotLightEffectFactory::VertexSpotLightEffectFactory()
    : EffectFactory(VertexSpotLightEffect::ID) {}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Effect> VertexSpotLightEffectFactory::create(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass,
    unique_ptr<ShaderProgram>& shaderProgram,
    const vector<shared_ptr<Texture2D>>&) const
{
    return make_shared<VertexSpotLightEffect>(graphicsDevice, renderPass, shaderProgram);
}

// ---------------------------------------------------------------------------------------------------------------------
