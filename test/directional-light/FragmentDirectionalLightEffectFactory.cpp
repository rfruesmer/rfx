#include "rfx/pch.h"
#include "test/directional-light/FragmentDirectionalLightEffectFactory.h"
#include "test/directional-light/FragmentDirectionalLightEffect.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

FragmentDirectionalLightEffectFactory::FragmentDirectionalLightEffectFactory()
    : EffectFactory(FragmentDirectionalLightEffect::ID) {}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Effect> FragmentDirectionalLightEffectFactory::create(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass, 
    unique_ptr<ShaderProgram>& shaderProgram,
    const vector<shared_ptr<Texture2D>>&) const
{
    return make_shared<FragmentDirectionalLightEffect>(graphicsDevice, renderPass, shaderProgram);
}

// ---------------------------------------------------------------------------------------------------------------------
