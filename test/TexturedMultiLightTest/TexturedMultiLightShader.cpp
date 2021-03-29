#include "rfx/pch.h"
#include "TexturedMultiLightShader.h"


using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

const std::string TexturedMultiLightShader::ID = "textured_multilight";

// ---------------------------------------------------------------------------------------------------------------------

TexturedMultiLightShader::TexturedMultiLightShader(const GraphicsDevicePtr& graphicsDevice)
    : TestMaterialShader(
        graphicsDevice,
        ID,
        ID,
        ID) {}

// ---------------------------------------------------------------------------------------------------------------------

vector<std::byte> TexturedMultiLightShader::createDataFor(const MaterialPtr& material) const
{
    const MaterialData materialData {
        .baseColor = material->getBaseColorFactor(),
        .specular = material->getSpecularFactor(),
        .shininess = material->getShininess()
    };

    vector<std::byte> data(sizeof(MaterialData));
    memcpy(data.data(), &materialData, sizeof(MaterialData));

    return data;
}

// ---------------------------------------------------------------------------------------------------------------------

const void* TexturedMultiLightShader::getData() const
{
    return reinterpret_cast<const void*>(&data);
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t TexturedMultiLightShader::getDataSize() const
{
    return sizeof(ShaderData);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightShader::setCamera(CameraPtr camera)
{
    this->camera = move(camera);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightShader::setLight(int index, const PointLightPtr& light)
{
    lights[index] = light;

    if (light == nullptr) {
        data.lights[index].enabled = false;
        return;
    }

    auto& lightData = data.lights[index];
    lightData.type = light->getType();
    lightData.enabled = true;
    lightData.color = vec4(light->getColor(), 1.0f);
    lightData.position = camera->getViewMatrix() * vec4(light->getPosition(), 1.0f);
    lightData.range = light->getRange();
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightShader::setLight(int index, const SpotLightPtr& light)
{
    setLight(index, static_pointer_cast<PointLight>(light));
    if (light == nullptr) {
        return;
    }

    auto& lightData = data.lights[index];
    lightData.direction = light->getDirection();
    lightData.spotInnerConeAngle = glm::cos(glm::radians(light->getInnerConeAngle()));
    lightData.spotOuterConeAngle = glm::cos(glm::radians(light->getOuterConeAngle()));
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightShader::onViewMatrixChanged()
{
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (lights[i] != nullptr) {
            data.lights[i].position = camera->getViewMatrix() * vec4(lights[i]->getPosition(), 1.0f);
        }
    }

    updateDataBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------
