#include "rfx/pch.h"
#include "PBREffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


const std::string PBREffect::VERTEX_SHADER_ID = "pbr";
const std::string PBREffect::FRAGMENT_SHADER_ID = "pbr";

// ---------------------------------------------------------------------------------------------------------------------

PBREffect::PBREffect(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    const shared_ptr<Model>& scene)
        : TestMaterialShader(graphicsDevice, scene) {}

// ---------------------------------------------------------------------------------------------------------------------

void PBREffect::setProjectionMatrix(const mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void PBREffect::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (lights_[i] != nullptr) {
            sceneData_.lights[i].position = viewMatrix * vec4(lights_[i]->getPosition(), 1.0f);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void PBREffect::setLight(int index, const shared_ptr<PointLight>& light)
{
    lights_[index] = light;
    if (light == nullptr) {
        sceneData_.lights[index].enabled = false;
        return;
    }

    auto& sceneDataLight = sceneData_.lights[index];
    sceneDataLight.enabled = true;
    sceneDataLight.position = sceneData_.viewMatrix * vec4(light->getPosition(), 1.0f);

    const vec3& lightColor = light->getColor();
    sceneDataLight.color = vec4(lightColor * 255.0f, 1.0f);
}
// ---------------------------------------------------------------------------------------------------------------------

size_t PBREffect::getSceneDataSize() const
{
    return sizeof(SceneData);
}

// ---------------------------------------------------------------------------------------------------------------------

void PBREffect::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------

void PBREffect::update(const shared_ptr<Material>& material) const
{
    MaterialData materialData {
        .baseColor = material->getBaseColorFactor(),
        .metallic = material->getMetallicFactor(),
        .roughness = material->getRoughnessFactor(),
        .ao = material->getOcclusionStrength()
    };

    const shared_ptr<Buffer>& uniformBuffer = material->getUniformBuffer();
    uniformBuffer->load(sizeof(MaterialData),
        reinterpret_cast<const void*>(&materialData));
}

// ---------------------------------------------------------------------------------------------------------------------

