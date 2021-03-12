#include "rfx/pch.h"
#include "TexturedPBREffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

TexturedPBREffect::TexturedPBREffect(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    const shared_ptr<Scene>& scene)
        : TestEffect(graphicsDevice, scene) {}

// ---------------------------------------------------------------------------------------------------------------------

string TexturedPBREffect::getVertexShaderFileName() const
{
    return "pbr_textured.vert";
}

// ---------------------------------------------------------------------------------------------------------------------

string TexturedPBREffect::getFragmentShaderFileName() const
{
    return "pbr_textured.frag";
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBREffect::setProjectionMatrix(const mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBREffect::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (lights_[i] != nullptr) {
            sceneData_.lights[i].position = vec4(lights_[i]->getPosition(), 1.0f);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBREffect::setLight(int index, const shared_ptr<PointLight>& light)
{
    lights_[index] = light;
    if (light == nullptr) {
        sceneData_.lights[index].enabled = false;
        return;
    }

    auto& sceneDataLight = sceneData_.lights[index];
    sceneDataLight.enabled = true;
    sceneDataLight.position = vec4(light->getPosition(), 1.0f);

    const vec3& lightColor = light->getColor();
    sceneDataLight.color = vec4(lightColor * 255.0f, 1.0f);
}
// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBREffect::setMetallicFactor(float factor)
{
    materialData_.metallic = factor;
}

// ---------------------------------------------------------------------------------------------------------------------

float TexturedPBREffect::getMetallicFactor() const
{
    return materialData_.metallic;
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBREffect::setRoughnessFactor(float factor)
{
    materialData_.roughness = factor;
}

// ---------------------------------------------------------------------------------------------------------------------

float TexturedPBREffect::getRoughnessFactor() const
{
    return materialData_.roughness;
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBREffect::setAmbientOcclusion(float value)
{
    materialData_.ao = value;
}

// ---------------------------------------------------------------------------------------------------------------------

float TexturedPBREffect::getAmbientOcclusion()
{
    return materialData_.ao;
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBREffect::createMaterialDataBuffers()
{
    MaterialData materialData {};

    for (const auto& material : scene_->getMaterials()) {
        shared_ptr<Buffer> materialUniformBuffer = graphicsDevice_->createBuffer(
            sizeof(MaterialData),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        graphicsDevice_->bind(materialUniformBuffer);
        materialUniformBuffer->load(sizeof(MaterialData), &materialData);
        materialDataBuffers_.push_back(materialUniformBuffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

size_t TexturedPBREffect::getSceneDataSize() const
{
    return sizeof(SceneData);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBREffect::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBREffect::updateMaterialDataBuffers()
{
    for (auto& materialDataBuffer : materialDataBuffers_) {
        materialDataBuffer->load(sizeof(MaterialData), &materialData_);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBREffect::setCameraPos(const vec3& pos)
{
    sceneData_.camPos = pos;
}

// ---------------------------------------------------------------------------------------------------------------------
