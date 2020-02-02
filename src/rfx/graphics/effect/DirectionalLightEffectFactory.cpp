#include "rfx/pch.h"
#include "rfx/graphics/effect/DirectionalLightEffectFactory.h"
#include "rfx/graphics/effect/DirectionalLightEffect.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

DirectionalLightEffectFactory::DirectionalLightEffectFactory()
    : EffectFactory(DirectionalLightEffect::ID) {}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Effect> DirectionalLightEffectFactory::create(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass, 
    unique_ptr<ShaderProgram>& shaderProgram,
    const vector<shared_ptr<Texture2D>>&) const
{
    return make_shared<DirectionalLightEffect>(graphicsDevice, renderPass, shaderProgram);
}

// ---------------------------------------------------------------------------------------------------------------------
