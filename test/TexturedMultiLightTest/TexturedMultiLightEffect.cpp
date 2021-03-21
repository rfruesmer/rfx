#include "rfx/pch.h"
#include "TexturedMultiLightEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


const std::string TexturedMultiLightEffect::VERTEX_SHADER_ID = "textured_multilight";
const std::string TexturedMultiLightEffect::FRAGMENT_SHADER_ID = "textured_multilight";

// ---------------------------------------------------------------------------------------------------------------------

TexturedMultiLightEffect::TexturedMultiLightEffect(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    const shared_ptr<Model>& scene)
        : TestEffect(graphicsDevice, scene) {}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightEffect::setProjectionMatrix(const glm::mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightEffect::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (lights_[i] != nullptr) {
            sceneData_.lights[i].position = viewMatrix * vec4(lights_[i]->getPosition(), 1.0f);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightEffect::setLight(int index, const shared_ptr<PointLight>& light)
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

void TexturedMultiLightEffect::setLight(int index, const shared_ptr<SpotLight>& light)
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

size_t TexturedMultiLightEffect::getSceneDataSize() const
{
    return sizeof(SceneData);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightEffect::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------
