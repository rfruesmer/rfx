#include "rfx/pch.h"
#include "SpotLightEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


const string SpotLightEffect::VERTEX_SHADER_ID = "spotlight";
const string SpotLightEffect::FRAGMENT_SHADER_ID = "spotlight";

// ---------------------------------------------------------------------------------------------------------------------

SpotLightEffect::SpotLightEffect(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    const shared_ptr<Model>& scene)
        : TestMaterialShader(graphicsDevice, scene) {}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightEffect::setProjectionMatrix(const glm::mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightEffect::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;
    sceneData_.lightPos = viewMatrix * vec4(light_->getPosition(), 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightEffect::setLight(const shared_ptr<SpotLight>& light)
{
    light_ = light;

    sceneData_.lightPos = sceneData_.viewMatrix * vec4(light->getPosition(), 1.0f);
    sceneData_.lightColor = vec4(light->getColor(), 1.0f);
    sceneData_.spotDirection = light->getDirection();
    sceneData_.spotInnerConeAngle = glm::cos(glm::radians(light->getInnerConeAngle()));
    sceneData_.spotOuterConeAngle = glm::cos(glm::radians(light->getOuterConeAngle()));
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SpotLightEffect::getSceneDataSize() const
{
    return sizeof(SceneData);
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightEffect::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightEffect::update(const shared_ptr<Material>& material) const
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

