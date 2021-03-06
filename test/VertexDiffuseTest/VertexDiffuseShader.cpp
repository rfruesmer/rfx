#include "rfx/pch.h"
#include "VertexDiffuseShader.h"


using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

const string VertexDiffuseShader::ID = "vertex_diffuse";

// ---------------------------------------------------------------------------------------------------------------------

VertexDiffuseShader::VertexDiffuseShader(const GraphicsDevicePtr& graphicsDevice)
    : TestMaterialShader(
        graphicsDevice,
        ID,
        ID,
        ID) {}

// ---------------------------------------------------------------------------------------------------------------------

vector<std::byte> VertexDiffuseShader::createDataFor(const MaterialPtr& material) const
{
    const MaterialData materialData {
        .baseColor = material->getBaseColorFactor()
    };

    vector<std::byte> data(sizeof(MaterialData));
    memcpy(data.data(), &materialData, sizeof(MaterialData));

    return data;
}

// ---------------------------------------------------------------------------------------------------------------------

const void* VertexDiffuseShader::getData() const
{
    return reinterpret_cast<const void*>(&data);
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t VertexDiffuseShader::getDataSize() const
{
    return sizeof(ShaderData);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseShader::setLightPosition(const vec3& position)
{
    data.lightPos = position;
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseShader::setLightAmbient(const vec3& color)
{
    data.La = vec4(color, 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseShader::setLightDiffuse(const vec3& color)
{
    data.Ld = vec4(color, 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseShader::setLightSpecular(const vec3& color)
{
    data.Ls = vec4(color, 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------
