#include "rfx/pch.h"
#include "NormalMapShader.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

const std::string NormalMapShader::ID = "normalmap";

// ---------------------------------------------------------------------------------------------------------------------

NormalMapShader::NormalMapShader(const GraphicsDevicePtr& graphicsDevice)
    : TestMaterialShader(
        graphicsDevice,
        ID,
        ID,
        ID) {}

// ---------------------------------------------------------------------------------------------------------------------

vector<std::byte> NormalMapShader::createDataFor(const MaterialPtr& material) const
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

const void* NormalMapShader::getData() const
{
    return reinterpret_cast<const void*>(&data);
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t NormalMapShader::getDataSize() const
{
    return sizeof(ShaderData);
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapShader::setLight(int index, const PointLightPtr& light)
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
    lightData.position = vec4(light->getPosition(), 1.0f);
    lightData.range = light->getRange();
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapShader::enableNormalMap(bool state)
{
    data.useNormalMap = state;
}

// ---------------------------------------------------------------------------------------------------------------------
