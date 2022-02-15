#pragma once

#include "TestMaterialShader.h"
#include "rfx/scene/DirectionalLight.h"

namespace rfx {

class SampleViewerShader : public TestMaterialShader
{
public:
    struct MaterialData {
        glm::vec4 baseColorFactor { 1.0f };
        float metallicFactor = 1.0f;
        float roughnessFactor = 1.0f;
        float pad0;
        float pad1;
    };

    static const std::string ID;
    static const int MAX_LIGHTS = 8;

    explicit SampleViewerShader(const GraphicsDevicePtr& graphicsDevice);

    [[nodiscard]] std::vector<std::byte> createDataFor(const MaterialPtr& material) const override;
    [[nodiscard]] const void* getData() const override;
    [[nodiscard]] uint32_t getDataSize() const override;

    std::vector<std::string> getShaderDefinesFor(const MaterialPtr& material) override;
    std::vector<std::string> getVertexShaderInputsFor(const MaterialPtr& material) override;
    std::vector<std::string> getVertexShaderOutputsFor(const MaterialPtr& material) override;
    std::vector<std::string> getFragmentShaderInputsFor(const MaterialPtr& material) override;

    void setLight(size_t index, const LightPtr& light);

private:
    struct LightData {
        glm::vec3 direction;
        float range;

        glm::vec3 color;
        float intensity = 1.0f;

        glm::vec3 position;
        float innerConeCos;

        float outerConeCos;
        int type;
        bool enabled = false;
        float pad;
    };

    struct ShaderData {
        LightData lights[MAX_LIGHTS];
    };

    void setDirectionalLight(
        const DirectionalLightPtr& light,
        LightData* outLightData);


    ShaderData data {};
};

} // namespace rfx