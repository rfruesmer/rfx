#include "rfx/pch.h"
#include "FragmentPhongEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

FragmentPhongEffect::FragmentPhongEffect(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<Scene>& scene)
        : TestEffect(graphicsDevice, scene) {}

// ---------------------------------------------------------------------------------------------------------------------

string FragmentPhongEffect::getVertexShaderFileName() const
{
    return "fragment_phong.vert";
}

// ---------------------------------------------------------------------------------------------------------------------

string FragmentPhongEffect::getFragmentShaderFileName() const
{
    return "fragment_phong.frag";
}

// ---------------------------------------------------------------------------------------------------------------------

