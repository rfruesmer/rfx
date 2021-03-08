#include "rfx/pch.h"
#include "VertexDiffuseEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

VertexDiffuseEffect::VertexDiffuseEffect(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<Scene>& scene)
        : TestEffect(graphicsDevice, scene),
          light_("point") {}

// ---------------------------------------------------------------------------------------------------------------------

string VertexDiffuseEffect::getVertexShaderFileName() const
{
    return "vertex_diffuse.vert";
}

// ---------------------------------------------------------------------------------------------------------------------

string VertexDiffuseEffect::getFragmentShaderFileName() const
{
    return "vertex_diffuse.frag";
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::setProjectionMatrix(const glm::mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;
    sceneData_.lightPos = viewMatrix * vec4(light_.getPosition(), 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::setLight(const PointLight& light)
{
    light_ = light;

    sceneData_.lightPos = sceneData_.viewMatrix * vec4(light.getPosition(), 1.0f);
    sceneData_.La = vec4(0.01f, 0.01f, 0.01f, 1.0f);
    sceneData_.Ld = vec4(0.7f, 0.7f, 0.7f, 1.0f);
    sceneData_.Ls = vec4(0.3f, 0.3f, 0.3f, 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t VertexDiffuseEffect::getSceneDataSize() const
{
    return sizeof(SceneData);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------