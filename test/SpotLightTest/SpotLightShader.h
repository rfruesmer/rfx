#pragma once

#include "TestMaterialShader.h"
#include "rfx/scene/SpotLight.h"


namespace rfx {

class SpotLightShader : public TestMaterialShader
{
public:
    static const std::string ID;

    explicit SpotLightShader(const GraphicsDevicePtr& graphicsDevice);

    [[nodiscard]] std::vector<std::byte> createDataFor(const MaterialPtr& material) const override;

    [[nodiscard]] const void* getData() const override;
    [[nodiscard]] uint32_t getDataSize() const override;

    void setLight(
        const glm::vec3& position,
        const glm::vec3& color,
        const glm::vec3& direction,
        float innerConeAngle,
        float outerConeAngle);

private:
    struct ShaderData {
        glm::vec3 lightPos;
        [[maybe_unused]] float pad1;
        glm::vec3 lightColor;
        [[maybe_unused]] float pad2;
        glm::vec3 spotDirection;
        [[maybe_unused]] float pad3;
        float spotInnerConeAngle = 0.0f;
        float spotOuterConeAngle = glm::radians(90.0f);
    };

    struct MaterialData {
        glm::vec4 baseColor;
        glm::vec3 specular;
        float shininess = 0.0f;
    };

    ShaderData data {};
};

using SpotLightShaderPtr = std::shared_ptr<SpotLightShader>;

} // namespace rfx