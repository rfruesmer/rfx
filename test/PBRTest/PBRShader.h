#pragma once

#include "TestMaterialShader.h"
#include "rfx/scene/PointLight.h"


namespace rfx {

class PBRShader : public TestMaterialShader
{
public:
    struct MaterialData {
        glm::vec3 baseColor { 0.0f };
        float pad0 = 0.0f;

        float metallic = 0.5f;
        float roughness = 0.2f;
        float ao = 0.0f;
        float pad1 = 0.0f;
    };

    static const std::string ID;
    static const int MAX_LIGHTS = 4;

    explicit PBRShader(const GraphicsDevicePtr& graphicsDevice);

    [[nodiscard]] std::vector<std::byte> createDataFor(const MaterialPtr& material) const override;

    [[nodiscard]] const void* getData() const override;
    [[nodiscard]] uint32_t getDataSize() const override;

    void setLight(
        int index,
        const PointLightPtr& light,
        const glm::mat4& viewMatrix);

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
        LightData lights[MAX_LIGHTS];
    };

    ShaderData data {};
};

} // namespace rfx