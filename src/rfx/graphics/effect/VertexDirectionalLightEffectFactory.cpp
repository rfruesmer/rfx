#include "rfx/pch.h"
#include "rfx/graphics/effect/VertexDirectionalLightEffectFactory.h"
#include "rfx/graphics/effect/VertexDirectionalLightEffect.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

VertexDirectionalLightEffectFactory::VertexDirectionalLightEffectFactory()
    : EffectFactory(VertexDirectionalLightEffect::ID) {}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Effect> VertexDirectionalLightEffectFactory::create(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass, 
    unique_ptr<ShaderProgram>& shaderProgram,
    const vector<shared_ptr<Texture2D>>&) const
{
    return make_shared<VertexDirectionalLightEffect>(graphicsDevice, renderPass, shaderProgram);
}

// ---------------------------------------------------------------------------------------------------------------------
