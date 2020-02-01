#include "rfx/pch.h"
#include "rfx/graphics/effect/VertexColorEffectFactory.h"
#include "rfx/graphics/effect/VertexColorEffect.h"


using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

VertexColorEffectFactory::VertexColorEffectFactory()
    : EffectFactory("vertex_color") {}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Effect> VertexColorEffectFactory::create(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass,
    unique_ptr<ShaderProgram>& shaderProgram,
    const vector<shared_ptr<Texture2D>>&) const
{
    return make_shared<VertexColorEffect>(graphicsDevice, renderPass, shaderProgram);
}

// ---------------------------------------------------------------------------------------------------------------------
