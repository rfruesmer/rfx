#include "rfx/pch.h"
#include "SpotLightShader.h"


using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

const string SpotLightShader::ID = "spotlight";

// ---------------------------------------------------------------------------------------------------------------------

SpotLightShader::SpotLightShader(const GraphicsDevicePtr& graphicsDevice)
    : TestMaterialShader(
        graphicsDevice,
        ID,
        ID,
        ID) {}

// ---------------------------------------------------------------------------------------------------------------------

vector<std::byte> SpotLightShader::createDataFor(const MaterialPtr& material) const
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

const void* SpotLightShader::getData() const
{
    return reinterpret_cast<const void*>(&data);
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t SpotLightShader::getDataSize() const
{
    return sizeof(ShaderData);
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightShader::setLight(
    const vec3& position,
    const vec3& color,
    const vec3& direction,
    float innerConeAngle,
    float outerConeAngle)
{
    data.lightPos = position;
    data.lightColor = vec4(color, 1.0f);
    data.spotDirection = direction;
    data.spotInnerConeAngle = glm::cos(glm::radians(innerConeAngle));
    data.spotOuterConeAngle = glm::cos(glm::radians(outerConeAngle));
}

// ---------------------------------------------------------------------------------------------------------------------
