#pragma once

#include "TestEffect.h"
#include "rfx/scene/Scene.h"
#include "rfx/scene/SpotLight.h"


namespace rfx {

class SpotLightEffect : public TestEffect
{
public:
    static inline const VertexFormat VERTEX_FORMAT {
        VertexFormat::COORDINATES | VertexFormat::NORMALS
    };

    SpotLightEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<Scene>& scene);

    void setProjectionMatrix(const glm::mat4& projection);
    void setViewMatrix(const glm::mat4& viewMatrix);
    void setLight(const SpotLight& light);

    [[nodiscard]] size_t getSceneDataSize() const override;
    void updateSceneDataBuffer();

    [[nodiscard]] VertexFormat getVertexFormat() const override { return VERTEX_FORMAT; };
    [[nodiscard]] std::string getVertexShaderFileName() const override;
    [[nodiscard]] std::string getFragmentShaderFileName() const override;

private:
    struct SceneData {
        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;
        glm::vec4 lightPos;          // light position in eye coords
        glm::vec4 La;                // Ambient light intensity
        glm::vec4 Ld;                // Diffuse light intensity
        glm::vec4 Ls;                // Specular light intensity
        glm::vec3 spotDirection;     // Direction of the spotlight in eye coords
        float spotExponent = 0.0f;   // Angular attenuation exponent
        float spotCutoff = 0.0f;     // Cutoff angle (0-90 in radians)
    };

    SceneData sceneData_ {};
    SpotLight light_;
};

} // namespace rfx