#include "rfx/pch.h"
#include "PointLightEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

PointLightEffect::PointLightEffect(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<Scene>& scene)
        : TestEffect(graphicsDevice, scene) {}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightEffect::setProjectionMatrix(const glm::mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightEffect::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;
    sceneData_.lightPos = viewMatrix * vec4(light_->getPosition(), 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightEffect::setLight(const shared_ptr<PointLight>& light)
{
    light_ = light;

    sceneData_.lightPos = sceneData_.viewMatrix * vec4(light->getPosition(), 1.0f);
    sceneData_.lightColor = vec4(light->getColor(), 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t PointLightEffect::getSceneDataSize() const
{
    return sizeof(SceneData);
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightEffect::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------
