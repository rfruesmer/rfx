#include "rfx/pch.h"
#include "VertexDiffuseShader.h"


using namespace rfx;
using namespace glm;
using namespace std;

const string VertexDiffuseShader::ID = "vertex_diffuse";

// ---------------------------------------------------------------------------------------------------------------------

VertexDiffuseShader::VertexDiffuseShader(const GraphicsDevicePtr& graphicsDevice)
        : TestMaterialShader(
            graphicsDevice,
            ID,
            ID,
            ID) {}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::byte> VertexDiffuseShader::createDataFor(const MaterialPtr& material) const
{
    const MaterialData materialData {
        .baseColor = material->getBaseColorFactor(),
        .specular = material->getSpecularFactor(),
        .shininess = material->getShininess()
    };

    std::vector<std::byte> data(sizeof(MaterialData));
    memcpy(data.data(), &materialData, sizeof(MaterialData));

    return data;
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseShader::update(const MaterialPtr& material) const
{
    vector<std::byte> materialData = createDataFor(material);
    
    const shared_ptr<Buffer>& uniformBuffer = material->getUniformBuffer();
    uniformBuffer->load(sizeof(MaterialData),
        reinterpret_cast<const void*>(materialData.data()));
}

// ---------------------------------------------------------------------------------------------------------------------
