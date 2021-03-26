#pragma once

#include "TestMaterialShader.h"
#include "rfx/scene/Model.h"
#include "rfx/scene/PointLight.h"
#include "rfx/scene/SpotLight.h"


namespace rfx {

class TexturedPBREffect : public TestMaterialShader
{
public:
    struct MaterialData {
        glm::vec4 baseColorFactor { 0.0f };
        glm::vec4 emissiveFactor { 0.0f };
        float metallic = 0.0f;
        float roughness = 0.0f;
        int baseColorTexCoordSet = -1;
        int metallicRoughnessTexCoordSet = -1;
        int normalTexCoordSet = -1;
        int occlusionTexCoordSet = -1;
        float occlusionStrength = 1.0f;
        int emissiveTexCoordSet = -1;
    };

    static const int MAX_LIGHTS = 4;
    static const std::string VERTEX_SHADER_ID;
    static const std::string FRAGMENT_SHADER_ID;

    TexturedPBREffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<Model>& scene);

    void setProjectionMatrix(const glm::mat4& projection);
    void setViewMatrix(const glm::mat4& viewMatrix);
    void setCameraPos(const glm::vec3& pos);
    void setLight(int index, const std::shared_ptr<PointLight>& light);

    void updateSceneDataBuffer();

    void update(const std::shared_ptr<Material>& material) const override;

    [[nodiscard]] std::vector<std::string> buildShaderDefines(
        const std::shared_ptr<Material>& material,
        const VertexFormat& vertexFormat) override;
    [[nodiscard]] std::vector<std::string> buildVertexShaderInputs(const VertexFormat& vertexFormat) override;
    [[nodiscard]] std::vector<std::string> buildVertexShaderOutputs(const VertexFormat& vertexFormat) override;
    [[nodiscard]] std::vector<std::string> buildFragmentShaderInputs(const VertexFormat& vertexFormat) override;

protected:
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

    SceneData sceneData_ {};
    std::shared_ptr<PointLight> lights_[MAX_LIGHTS];
};

} // namespace rfx