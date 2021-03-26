#pragma once

#include "TestMaterialShader.h"
#include "rfx/scene/Model.h"
#include "rfx/scene/PointLight.h"


namespace rfx {

class PointLightEffect : public TestMaterialShader
{
public:
    struct MaterialData {
        glm::vec4 baseColor;
        glm::vec3 specular;
        float shininess = 0.0f;
    };

    static const std::string VERTEX_SHADER_ID;
    static const std::string FRAGMENT_SHADER_ID;

    PointLightEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<Model>& scene);

    void setProjectionMatrix(const glm::mat4& projection);
    void setViewMatrix(const glm::mat4& viewMatrix);
    void setLight(const std::shared_ptr<PointLight>& light);

    [[nodiscard]] size_t getSceneDataSize() const override;
    void updateSceneDataBuffer();
    void update(const std::shared_ptr<Material>& material) const override;

private:
    struct SceneData {
        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;
        glm::vec3 lightPos;          // light position in eye coords
        [[maybe_unused]] float pad;
        glm::vec3 lightColor;
    };

    SceneData sceneData_ {};
    std::shared_ptr<PointLight> light_;
};

} // namespace rfx