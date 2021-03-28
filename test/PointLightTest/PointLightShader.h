#pragma once

#include "TestMaterialShader.h"
#include "rfx/scene/PointLight.h"


namespace rfx {

class PointLightShader : public TestMaterialShader
{
public:
    static const std::string ID;

    explicit PointLightShader(const GraphicsDevicePtr& graphicsDevice);

    [[nodiscard]] std::vector<std::byte> createDataFor(const MaterialPtr& material) const override;

    [[nodiscard]] const void* getData() const override;
    [[nodiscard]] uint32_t getDataSize() const override;

    void setLightPosition(const glm::vec3& position);
    void setLightColor(const glm::vec3& color);

private:
    struct ShaderData {
        glm::vec3 lightPos { 0.0f };
        [[maybe_unused]] float pad;
        glm::vec3 lightColor { 1.0f };
    };

    struct MaterialData {
        glm::vec4 baseColor;
        glm::vec3 specular;
        float shininess = 0.0f;
    };

    ShaderData data {};
};

using PointLightShaderPtr = std::shared_ptr<PointLightShader>;

} // namespace rfx