#include "rfx/pch.h"
#include "NormalMapEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

NormalMapEffect::NormalMapEffect(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<Scene>& scene)
        : TestEffect(graphicsDevice, scene) {}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapEffect::setProjectionMatrix(const glm::mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapEffect::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (lights_[i] != nullptr) {
            sceneData_.lights[i].position = vec4(lights_[i]->getPosition(), 1.0f);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapEffect::setCameraPosition(const vec3& position)
{
    sceneData_.camPos = position;
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapEffect::setLight(int index, const shared_ptr<PointLight>& light)
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
    sceneDataLight.position = vec4(light->getPosition(), 1.0f);
    sceneDataLight.range = light->getRange();
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapEffect::enableNormalMap(bool state)
{
    sceneData_.useNormalMap = state;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t NormalMapEffect::getSceneDataSize() const
{
    return sizeof(SceneData);
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapEffect::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------
