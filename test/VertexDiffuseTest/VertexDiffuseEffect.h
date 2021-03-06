#pragma once

#include "TestEffect.h"
#include "rfx/scene/PointLight.h"


namespace rfx {

class VertexDiffuseEffect : public TestEffect
{
public:
    static inline const VertexFormat VERTEX_FORMAT {
        VertexFormat::COORDINATES | VertexFormat::NORMALS
    };

    VertexDiffuseEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<Scene>& scene);

    [[nodiscard]] VertexFormat getVertexFormat() const override { return VERTEX_FORMAT; };
    [[nodiscard]] std::string getVertexShaderFileName() const override;
    [[nodiscard]] std::string getFragmentShaderFileName() const override;

    void setProjectionMatrix(const glm::mat4& projection);
    void setViewMatrix(const glm::mat4& viewMatrix);
    void setLight(const PointLight& light);

    [[nodiscard]] size_t getSceneDataSize() const override;
    void updateSceneDataBuffer();


private:
    struct SceneData {
        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;
        glm::vec3 lightPos;          // light position in eye coords
        [[maybe_unused]] float pad;
        glm::vec4 La;
        glm::vec4 Ld;
        glm::vec4 Ls;
    };

    SceneData sceneData_ {};
    PointLight light_;
};

} // namespace rfx