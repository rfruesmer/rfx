#include "rfx/pch.h"
#include "rfx/graphics/effect/VertexPointLightEffectFactory.h"
#include "rfx/graphics/effect/VertexPointLightEffect.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

VertexPointLightEffectFactory::VertexPointLightEffectFactory()
    : EffectFactory(VertexPointLightEffect::ID) {}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Effect> VertexPointLightEffectFactory::create(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass,
    unique_ptr<ShaderProgram>& shaderProgram,
    const vector<shared_ptr<Texture2D>>&) const
{
    return make_shared<VertexPointLightEffect>(graphicsDevice, renderPass, shaderProgram);
}

// ---------------------------------------------------------------------------------------------------------------------
