#pragma once

#include "TestEffect.h"

namespace rfx {

class SkyBoxEffect : public TestEffect
{
public:
    static const std::string VERTEX_SHADER_ID;
    static const std::string FRAGMENT_SHADER_ID;

    SkyBoxEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<Model>& scene);

    void setProjectionMatrix(const glm::mat4& projection);
    void setViewMatrix(const glm::mat4& viewMatrix);

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
};

} // namespace rfx
