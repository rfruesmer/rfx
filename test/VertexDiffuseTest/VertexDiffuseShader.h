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

    void update(const std::shared_ptr<Material>& material) const override;
};

} // namespace rfx