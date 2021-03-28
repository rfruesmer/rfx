#include "rfx/pch.h"
#include "PointLightShader.h"


using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

const string PointLightShader::ID = "pointlight";

// ---------------------------------------------------------------------------------------------------------------------

PointLightShader::PointLightShader(const GraphicsDevicePtr& graphicsDevice)
    : TestMaterialShader(
        graphicsDevice,
        ID,
        ID,
        ID) {}

// ---------------------------------------------------------------------------------------------------------------------

vector<std::byte> PointLightShader::createDataFor(const MaterialPtr& material) const
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

const void* PointLightShader::getData() const
{
    return reinterpret_cast<const void*>(&data);
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t PointLightShader::getDataSize() const
{
    return sizeof(ShaderData);
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightShader::setLightPosition(const vec3& position)
{
    data.lightPos = position;
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightShader::setLightColor(const vec3& color)
{
    data.lightColor = color;
}

// ---------------------------------------------------------------------------------------------------------------------
