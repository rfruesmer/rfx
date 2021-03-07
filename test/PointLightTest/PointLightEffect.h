#pragma once

#include "TestEffect.h"
#include "rfx/scene/Scene.h"
#include "rfx/scene/PointLight.h"


namespace rfx {

class PointLightEffect : public TestEffect
{
public:
    static inline const VertexFormat VERTEX_FORMAT {
        VertexFormat::COORDINATES | VertexFormat::NORMALS
    };

    PointLightEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<Scene>& scene);

    void setProjectionMatrix(const glm::mat4& projection);
    void setViewMatrix(const glm::mat4& viewMatrix);
    void setLight(const std::shared_ptr<PointLight>& light);

    [[nodiscard]] size_t getSceneDataSize() const override;
    void updateSceneDataBuffer();

    [[nodiscard]] VertexFormat getVertexFormat() const override { return VERTEX_FORMAT; };
    [[nodiscard]] std::string getVertexShaderFileName() const override;
    [[nodiscard]] std::string getFragmentShaderFileName() const override;

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