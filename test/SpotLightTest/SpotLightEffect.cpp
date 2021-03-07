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
    sceneData_.lightPos = viewMatrix * vec4(light_->getPosition(), 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightEffect::setLight(const shared_ptr<SpotLight>& light)
{
    light_ = light;

    sceneData_.lightPos = sceneData_.viewMatrix * vec4(light->getPosition(), 1.0f);
    sceneData_.lightColor = vec4(light->getColor(), 1.0f);
    sceneData_.spotDirection = light->getDirection();
    sceneData_.spotInnerConeAngle = glm::cos(glm::radians(light->getInnerConeAngle()));
    sceneData_.spotOuterConeAngle = glm::cos(glm::radians(light->getOuterConeAngle()));
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

