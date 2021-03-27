#pragma once

#include "TestMaterialShader.h"
#include "rfx/scene/PointLight.h"


namespace rfx {

class VertexDiffuseShader : public TestMaterialShader
{
public:
    struct MaterialData {
        glm::vec4 baseColor;
        glm::vec3 specular;
        float shininess = 0.0f;
    };

    static const std::string ID;

    explicit VertexDiffuseShader(const GraphicsDevicePtr& graphicsDevice);

    [[nodiscard]] std::vector<std::byte> createDataFor(const MaterialPtr& material) const override;

    void update(const MaterialPtr& material) const override;
};

} // namespace rfx