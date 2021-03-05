#include "rfx/pch.h"
#include "SpotLightEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

SpotLightEffect::SpotLightEffect(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<Scene>& scene)
        : TestEffect(graphicsDevice, scene) {}

// ---------------------------------------------------------------------------------------------------------------------

string SpotLightEffect::getVertexShaderFileName() const
{
    return "spotlight.vert";
}

// ---------------------------------------------------------------------------------------------------------------------

string SpotLightEffect::getFragmentShaderFileName() const
{
    return "spotlight.frag";
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightEffect::setProjectionMatrix(const glm::mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightEffect::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;
    sceneData_.lightPos = viewMatrix * vec4(light_.getPosition(), 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightEffect::setLight(const SpotLight& light)
{
    light_ = light;

    sceneData_.lightPos = sceneData_.viewMatrix * vec4(light.getPosition(), 1.0f);
    sceneData_.La = vec4(light.getAmbient(), 1.0f);
    sceneData_.Ld = vec4(light.getDiffuse(), 1.0f);
    sceneData_.Ls = vec4(light.getSpecular(), 1.0f);
    sceneData_.spotDirection = light.getDirection();
    sceneData_.spotExponent = light.getExponent();
    sceneData_.spotCutoff = light.getCutoff();
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SpotLightEffect::getSceneDataSize() const
{
    return sizeof(SceneData);
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightEffect::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------
