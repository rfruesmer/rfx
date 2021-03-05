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

void FragmentPhongEffect::setProjectionMatrix(const glm::mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void FragmentPhongEffect::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;
    sceneData_.lightPos = viewMatrix * vec4(light_.getPosition(), 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void FragmentPhongEffect::setLight(const PointLight& light)
{
    light_ = light;

    sceneData_.lightPos = sceneData_.viewMatrix * vec4(light.getPosition(), 1.0f);
    sceneData_.La = vec4(light.getAmbient(), 1.0f);
    sceneData_.Ld = vec4(light.getDiffuse(), 1.0f);
    sceneData_.Ls = vec4(light.getSpecular(), 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t FragmentPhongEffect::getSceneDataSize() const
{
    return sizeof(SceneData);
}

// ---------------------------------------------------------------------------------------------------------------------

void FragmentPhongEffect::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------
