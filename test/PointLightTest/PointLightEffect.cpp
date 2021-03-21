#include "rfx/pch.h"
#include "PointLightEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


const string PointLightEffect::VERTEX_SHADER_ID = "pointlight";
const string PointLightEffect::FRAGMENT_SHADER_ID = "pointlight";

// ---------------------------------------------------------------------------------------------------------------------

PointLightEffect::PointLightEffect(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    const shared_ptr<Model>& scene)
        : TestEffect(graphicsDevice, scene) {}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightEffect::setProjectionMatrix(const glm::mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightEffect::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;
    sceneData_.lightPos = viewMatrix * vec4(light_->getPosition(), 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightEffect::setLight(const shared_ptr<PointLight>& light)
{
    light_ = light;

    sceneData_.lightPos = sceneData_.viewMatrix * vec4(light->getPosition(), 1.0f);
    sceneData_.lightColor = vec4(light->getColor(), 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t PointLightEffect::getSceneDataSize() const
{
    return sizeof(SceneData);
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightEffect::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightEffect::update(const shared_ptr<Material>& material) const
{
    const PointLightEffect::MaterialData materialData {
        .baseColor = material->getBaseColorFactor(),
        .specular = material->getSpecularFactor(),
        .shininess = material->getShininess()
    };

    const shared_ptr<Buffer>& uniformBuffer = material->getUniformBuffer();
    uniformBuffer->load(sizeof(MaterialData),
        reinterpret_cast<const void*>(&materialData));
}

// ---------------------------------------------------------------------------------------------------------------------
