#pragma once

#include "TestEffect.h"
#include "rfx/scene/PointLight.h"


namespace rfx {

class VertexDiffuseEffect : public TestEffect
{
public:
    static const std::string VERTEX_SHADER_ID;
    static const std::string FRAGMENT_SHADER_ID;

    VertexDiffuseEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<Model>& scene);

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