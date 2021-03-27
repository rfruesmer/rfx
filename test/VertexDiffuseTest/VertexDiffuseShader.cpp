#include "rfx/pch.h"
#include "VertexDiffuseShader.h"


using namespace rfx;
using namespace glm;
using namespace std;

const string VertexDiffuseShader::VERTEX_SHADER_ID = "vertex_diffuse";
const string VertexDiffuseShader::FRAGMENT_SHADER_ID = "vertex_diffuse";

// ---------------------------------------------------------------------------------------------------------------------

VertexDiffuseShader::VertexDiffuseShader(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    const shared_ptr<Model>& scene)
        : TestMaterialShader(graphicsDevice, scene) {}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseShader::update(const shared_ptr<Material>& material) const
{
    const MaterialData materialData {
        .baseColor = material->getBaseColorFactor(),
        .specular = material->getSpecularFactor(),
        .shininess = material->getShininess()
    };

    const shared_ptr<Buffer>& uniformBuffer = material->getUniformBuffer();
    uniformBuffer->load(sizeof(MaterialData),
        reinterpret_cast<const void*>(&materialData));
}

// ---------------------------------------------------------------------------------------------------------------------
