#pragma once

#include "TestEffect.h"
#include "rfx/scene/Scene.h"
#include "rfx/scene/PointLight.h"
#include "rfx/scene/SpotLight.h"


namespace rfx {

class TexturedPBREffect : public TestEffect
{
public:
    static const int MAX_LIGHTS = 4;

    static inline const VertexFormat VERTEX_FORMAT {
        VertexFormat::COORDINATES
            | VertexFormat::NORMALS
            | VertexFormat::TEXCOORDS
            | VertexFormat::TANGENTS
    };

    TexturedPBREffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<Scene>& scene);

    void setProjectionMatrix(const glm::mat4& projection);
    void setViewMatrix(const glm::mat4& viewMatrix);
    void setLight(int index, const std::shared_ptr<PointLight>& light);

    void setMetallicFactor(float factor);
    [[nodiscard]] float getMetallicFactor() const;

    void setRoughnessFactor(float factor);
    [[nodiscard]] float getRoughnessFactor() const;

    void setAmbientOcclusion(float value);
    [[nodiscard]] float getAmbientOcclusion();

    void updateSceneDataBuffer();

    [[nodiscard]] VertexFormat getVertexFormat() const override { return VERTEX_FORMAT; };
    [[nodiscard]] std::string getVertexShaderFileName() const override;
    [[nodiscard]] std::string getFragmentShaderFileName() const override;


    void updateMaterialDataBuffers();

    void setCameraPos(const glm::vec3& pos);

protected:
    void createMaterialDataBuffers() override;
    [[nodiscard]] size_t getSceneDataSize() const override;

private:
    struct LightData {
        glm::vec3 position { 0.0f };
        float pad0;

        glm::vec3 color { 1.0f };
        float pad1;

        bool enabled = false;
        float pad2;
        float pad3;
        float pad4;
    };

    struct SceneData {
        glm::mat4 viewMatrix { 1.0f };
        glm::mat4 projMatrix { 1.0f };

        glm::vec3 camPos;
        float padding;

        LightData lights[4];
    };

    struct MaterialData {
        float metallic = 0.5f;
        float roughness = 0.2f;
        float ao = 0.0f;
        float pad1;
    };

    SceneData sceneData_ {};
    MaterialData materialData_ {};
    std::shared_ptr<PointLight> lights_[MAX_LIGHTS];
};

} // namespace rfx