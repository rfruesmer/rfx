#include "rfx/pch.h"
#include "MultiLightEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


const std::string MultiLightEffect::VERTEX_SHADER_ID = "multilight";
const std::string MultiLightEffect::FRAGMENT_SHADER_ID = "multilight";

// ---------------------------------------------------------------------------------------------------------------------

MultiLightEffect::MultiLightEffect(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    const shared_ptr<Model>& scene)
        : TestEffect(graphicsDevice, scene) {}

// ---------------------------------------------------------------------------------------------------------------------

void MultiLightEffect::setProjectionMatrix(const glm::mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void MultiLightEffect::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (lights_[i] != nullptr) {
            sceneData_.lights[i].position = viewMatrix * vec4(lights_[i]->getPosition(), 1.0f);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MultiLightEffect::setLight(int index, const shared_ptr<PointLight>& light)
{
    lights_[index] = light;
    if (light == nullptr) {
        sceneData_.lights[index].enabled = false;
        return;
    }

    auto& sceneDataLight = sceneData_.lights[index];
    sceneDataLight.type = light->getType();
    sceneDataLight.enabled = true;
    sceneDataLight.color = vec4(light->getColor(), 1.0f);
    sceneDataLight.position = sceneData_.viewMatrix * vec4(light->getPosition(), 1.0f);
    sceneDataLight.range = light->getRange();
}

// ---------------------------------------------------------------------------------------------------------------------

void MultiLightEffect::setLight(int index, const shared_ptr<SpotLight>& light)
{
    setLight(index, static_pointer_cast<PointLight>(light));
    if (light == nullptr) {
        return;
    }

    auto& sceneDataLight = sceneData_.lights[index];
    sceneDataLight.direction = light->getDirection();
    sceneDataLight.spotInnerConeAngle = glm::cos(glm::radians(light->getInnerConeAngle()));
    sceneDataLight.spotOuterConeAngle = glm::cos(glm::radians(light->getOuterConeAngle()));
}

// ---------------------------------------------------------------------------------------------------------------------

size_t MultiLightEffect::getSceneDataSize() const
{
    return sizeof(SceneData);
}

// ---------------------------------------------------------------------------------------------------------------------

void MultiLightEffect::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------
