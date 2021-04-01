#pragma once

#include "TestMaterialShader.h"
#include "rfx/scene/Model.h"
#include "rfx/scene/PointLight.h"
#include "rfx/scene/SpotLight.h"


namespace rfx {

class TexturedPBRShader : public TestMaterialShader
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

    static const std::string ID;
    static const int MAX_LIGHTS = 4;

    explicit TexturedPBRShader(GraphicsDevicePtr& graphicsDevice);

    [[nodiscard]] std::vector<std::byte> createDataFor(const MaterialPtr& material) const override;

    [[nodiscard]] const void* getData() const override;
    [[nodiscard]] uint32_t getDataSize() const override;

    void setLight(int index, const PointLightPtr& light);

    std::vector<std::string> getShaderDefinesFor(const MaterialPtr& material) override;

    std::vector<std::string> getVertexShaderInputsFor(const MaterialPtr& material) override;

    std::vector<std::string> getVertexShaderOutputsFor(const MaterialPtr& material) override;

    std::vector<std::string> getFragmentShaderInputsFor(const MaterialPtr& material) override;

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

    struct ShaderData {
        LightData lights[4];
    };

    ShaderData data {};
    PointLightPtr lights[MAX_LIGHTS];
};

} // namespace rfx