#pragma once

#include "TestEffect.h"
#include "rfx/scene/Scene.h"
#include "rfx/scene/PointLight.h"


namespace rfx {

class NormalMapEffect : public TestEffect
{
public:
    static const int MAX_LIGHTS = 4;

    static inline const VertexFormat VERTEX_FORMAT {
        VertexFormat::COORDINATES
            | VertexFormat::NORMALS
            | VertexFormat::TEXCOORDS
            | VertexFormat::TANGENTS
    };

    NormalMapEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<Scene>& scene);

    void setProjectionMatrix(const glm::mat4& projection);
    void setViewMatrix(const glm::mat4& viewMatrix);
    void setLight(int index, const std::shared_ptr<PointLight>& light);
    void setCameraPosition(const glm::vec3& position);
    void enableNormalMap(bool state);

    [[nodiscard]] size_t getSceneDataSize() const override;
    void updateSceneDataBuffer();

    [[nodiscard]] VertexFormat getVertexFormat() const override { return VERTEX_FORMAT; };
    [[nodiscard]] std::string getVertexShaderFileName() const override;
    [[nodiscard]] std::string getFragmentShaderFileName() const override;

private:
    struct LightData {
        int type = Light::LightType::POINT;
        bool enabled = false;
        float pad1;
        float pad2;

        glm::vec3 position;
        float pad3;

        glm::vec3 color;
        float pad4;

        glm::vec3 direction;
        float pad5;

        float spotInnerConeAngle = 0.0f;
        float spotOuterConeAngle = 0.0f;
        float range = 0.0f;
        float pad6;
    };

    struct SceneData {
        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;

        glm::vec3 camPos;
        float padding;

        LightData lights[MAX_LIGHTS];

        int useNormalMap = 1;
    };

    SceneData sceneData_ {};
    std::shared_ptr<PointLight> lights_[MAX_LIGHTS];
};

} // namespace rfx