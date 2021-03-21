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
        : TestEffect(graphicsDevice, scene) {}

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

void PBREffect::setMetallicFactor(float factor)
{
    materialData_.metallic = factor;
}

// ---------------------------------------------------------------------------------------------------------------------

float PBREffect::getMetallicFactor() const
{
    return materialData_.metallic;
}

// ---------------------------------------------------------------------------------------------------------------------

void PBREffect::setRoughnessFactor(float factor)
{
    materialData_.roughness = factor;
}

// ---------------------------------------------------------------------------------------------------------------------

float PBREffect::getRoughnessFactor() const
{
    return materialData_.roughness;
}

// ---------------------------------------------------------------------------------------------------------------------

void PBREffect::setAlbedo(const vec3& value)
{
    materialData_.baseColor = value;
}

// ---------------------------------------------------------------------------------------------------------------------

vec3 PBREffect::getAlbedo() const
{
    return materialData_.baseColor;
}

// ---------------------------------------------------------------------------------------------------------------------

void PBREffect::setAmbientOcclusion(float value)
{
    materialData_.ao = value;
}

// ---------------------------------------------------------------------------------------------------------------------

float PBREffect::getAmbientOcclusion()
{
    return materialData_.ao;
}

// ---------------------------------------------------------------------------------------------------------------------

void PBREffect::createMaterialDataBuffers()
{
    for (const auto& material : scene_->getMaterials()) {
        shared_ptr<Buffer> materialUniformBuffer = graphicsDevice_->createBuffer(
            sizeof(MaterialData),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        MaterialData materialData {
            .pad0 = 0.0f
        };

        graphicsDevice_->bind(materialUniformBuffer);
        materialUniformBuffer->load(sizeof(MaterialData), &materialData);
        materialDataBuffers_.push_back(materialUniformBuffer);
    }
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

void PBREffect::updateMaterialDataBuffers()
{
    for (auto& materialDataBuffer : materialDataBuffers_) {
        materialDataBuffer->load(sizeof(MaterialData), &materialData_);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
