#include "rfx/pch.h"
#include "test/texture-mapping/Texture2DEffectFactory.h"
#include "test/texture-mapping/Texture2DEffect.h"


using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Texture2DEffectFactory::Texture2DEffectFactory()
    : EffectFactory(Texture2DEffect::ID) {}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Effect> Texture2DEffectFactory::create(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass,
    unique_ptr<ShaderProgram>& shaderProgram,
    const vector<shared_ptr<Texture2D>>& textures) const
{
    return make_shared<Texture2DEffect>(graphicsDevice, renderPass, shaderProgram, textures[0]);
}

// ---------------------------------------------------------------------------------------------------------------------
