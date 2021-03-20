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

    TexturedPBREffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<Scene>& scene);

    void setProjectionMatrix(const glm::mat4& projection);
    void setViewMatrix(const glm::mat4& viewMatrix);
    void setCameraPos(const glm::vec3& pos);
    void setLight(int index, const std::shared_ptr<PointLight>& light);

    void updateSceneDataBuffer();

    [[nodiscard]] VertexFormat getVertexFormat() const override;
    [[nodiscard]] std::string getVertexShaderFileName() const override;
    [[nodiscard]] std::string getFragmentShaderFileName() const override;
    [[nodiscard]] std::vector<std::string> buildShaderDefines(
        const std::shared_ptr<Material>& material,
        const VertexFormat& vertexFormat) override;
    [[nodiscard]] std::vector<std::string> buildVertexShaderInputs(const VertexFormat& vertexFormat) override;
    [[nodiscard]] std::vector<std::string> buildVertexShaderOutputs(const VertexFormat& vertexFormat) override;
    [[nodiscard]] std::vector<std::string> buildFragmentShaderInputs(const VertexFormat& vertexFormat) override;

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

    SceneData sceneData_ {};
    std::shared_ptr<PointLight> lights_[MAX_LIGHTS];
};

} // namespace rfx