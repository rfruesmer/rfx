#include "rfx/pch.h"
#include "PBRShader.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

const std::string PBRShader::ID = "pbr";

// ---------------------------------------------------------------------------------------------------------------------

PBRShader::PBRShader(const GraphicsDevicePtr& graphicsDevice)
    : TestMaterialShader(
        graphicsDevice,
        ID,
        ID,
        ID) {}

// ---------------------------------------------------------------------------------------------------------------------

vector<std::byte> PBRShader::createDataFor(const MaterialPtr& material) const
{
    MaterialData materialData {
        .baseColor = material->getBaseColorFactor(),
        .metallic = material->getMetallicFactor(),
        .roughness = material->getRoughnessFactor(),
        .ao = material->getOcclusionStrength()
    };

    vector<std::byte> data(sizeof(MaterialData));
    memcpy(data.data(), &materialData, sizeof(MaterialData));

    return data;
}

// ---------------------------------------------------------------------------------------------------------------------

const void* PBRShader::getData() const
{
    return reinterpret_cast<const void*>(&data);
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t PBRShader::getDataSize() const
{
    return sizeof(ShaderData);
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRShader::setLight(
    int index,
    const PointLightPtr& light,
    const mat4& viewMatrix)
{
    if (light == nullptr) {
        data.lights[index].enabled = false;
        return;
    }

    auto& lightData = data.lights[index];
    lightData.enabled = true;
    lightData.position = viewMatrix * vec4(light->getPosition(), 1.0f);
    lightData.color = vec4(light->getColor() * 255.0f, 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------
