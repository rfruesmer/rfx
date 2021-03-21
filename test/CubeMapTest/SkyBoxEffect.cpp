#include "rfx/pch.h"
#include "SkyBoxEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;

const string SkyBoxEffect::VERTEX_SHADER_ID = "skybox";
const string SkyBoxEffect::FRAGMENT_SHADER_ID = "skybox";

// ---------------------------------------------------------------------------------------------------------------------

SkyBoxEffect::SkyBoxEffect(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    const shared_ptr<Model>& scene)
        : TestEffect(graphicsDevice, scene) {}

// ---------------------------------------------------------------------------------------------------------------------

void SkyBoxEffect::setProjectionMatrix(const glm::mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void SkyBoxEffect::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SkyBoxEffect::getSceneDataSize() const
{
    return sizeof(SceneData);
}

// ---------------------------------------------------------------------------------------------------------------------

void SkyBoxEffect::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------
