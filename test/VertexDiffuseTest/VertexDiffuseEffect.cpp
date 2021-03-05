#include "rfx/pch.h"
#include "VertexDiffuseEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

VertexDiffuseEffect::VertexDiffuseEffect(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<Scene>& scene)
        : TestEffect(graphicsDevice, scene) {}

// ---------------------------------------------------------------------------------------------------------------------

string VertexDiffuseEffect::getVertexShaderFileName() const
{
    return "vertex_diffuse.vert";
}

// ---------------------------------------------------------------------------------------------------------------------

string VertexDiffuseEffect::getFragmentShaderFileName() const
{
    return "vertex_diffuse.frag";
}

// ---------------------------------------------------------------------------------------------------------------------

