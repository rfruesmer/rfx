#include "rfx/pch.h"
#include "ReflectionEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;

const string ReflectionEffect::VERTEX_SHADER_ID = "cubemap_reflection";
const string ReflectionEffect::FRAGMENT_SHADER_ID = "cubemap_reflection";

// ---------------------------------------------------------------------------------------------------------------------

ReflectionEffect::ReflectionEffect(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    const shared_ptr<Model>& scene)
        : TestMaterialShader(graphicsDevice, scene) {}

// ---------------------------------------------------------------------------------------------------------------------

void ReflectionEffect::setProjectionMatrix(const glm::mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void ReflectionEffect::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t ReflectionEffect::getSceneDataSize() const
{
    return sizeof(SceneData);
}

// ---------------------------------------------------------------------------------------------------------------------

void ReflectionEffect::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------
