#pragma once

#include "TestEffect.h"
#include "rfx/scene/Scene.h"
#include "rfx/scene/SpotLight.h"


namespace rfx {

class SpotLightEffect : public TestEffect
{
public:
    static const std::string VERTEX_SHADER_ID;
    static const std::string FRAGMENT_SHADER_ID;

    SpotLightEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<Scene>& scene);

    void setProjectionMatrix(const glm::mat4& projection);
    void setViewMatrix(const glm::mat4& viewMatrix);
    void setLight(const std::shared_ptr<SpotLight>& light);

    [[nodiscard]] size_t getSceneDataSize() const override;
    void updateSceneDataBuffer();

private:
    struct SceneData {
        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;
        glm::vec3 lightPos;          // light position in eye coords
        [[maybe_unused]] float pad1;
        glm::vec3 lightColor;
        [[maybe_unused]] float pad2;
        glm::vec3 spotDirection;     // Direction of the spotlight in eye coords
        [[maybe_unused]] float pad3;
        float spotInnerConeAngle = 0.0f;
        float spotOuterConeAngle = glm::radians(90.0f);
    };

    SceneData sceneData_ {};
    std::shared_ptr<SpotLight> light_;
};

} // namespace rfx