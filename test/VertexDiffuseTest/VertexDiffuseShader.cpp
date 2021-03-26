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
        : TestMaterialShader(graphicsDevice, scene),
          light_("point") {}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseShader::setProjectionMatrix(const glm::mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseShader::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;
    sceneData_.lightPos = viewMatrix * vec4(light_.getPosition(), 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseShader::setLight(const PointLight& light)
{
    light_ = light;

    sceneData_.lightPos = sceneData_.viewMatrix * vec4(light.getPosition(), 1.0f);
    sceneData_.La = vec4(0.01f, 0.01f, 0.01f, 1.0f);
    sceneData_.Ld = vec4(0.7f, 0.7f, 0.7f, 1.0f);
    sceneData_.Ls = vec4(0.3f, 0.3f, 0.3f, 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t VertexDiffuseShader::getSceneDataSize() const
{
    return sizeof(SceneData);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseShader::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

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
