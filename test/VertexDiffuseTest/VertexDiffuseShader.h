#pragma once

#include "TestMaterialShader.h"
#include "rfx/scene/PointLight.h"


namespace rfx {

class VertexDiffuseShader : public TestMaterialShader
{
public:
    static const std::string ID;

    explicit VertexDiffuseShader(const GraphicsDevicePtr& graphicsDevice);

    [[nodiscard]] std::vector<std::byte> createDataFor(const MaterialPtr& material) const override;

    [[nodiscard]] const void* getData() const override;
    [[nodiscard]] uint32_t getDataSize() const override;

    void setLightPosition(const glm::vec3& position);
    void setLightAmbient(const glm::vec3& color);
    void setLightDiffuse(const glm::vec3& color);
    void setLightSpecular(const glm::vec3& color);

private:
    struct ShaderData {
        glm::vec3 lightPos;
        [[maybe_unused]] float pad;
        glm::vec4 La;
        glm::vec4 Ld;
        glm::vec4 Ls;
    };

    struct MaterialData {
        glm::vec4 baseColor { 0.0f };
    };

    ShaderData data {};
};

using VertexDiffuseShaderPtr = std::shared_ptr<VertexDiffuseShader>;

} // namespace rfx