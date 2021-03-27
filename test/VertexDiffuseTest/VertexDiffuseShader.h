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

    static const std::string VERTEX_SHADER_ID;
    static const std::string FRAGMENT_SHADER_ID;

    explicit VertexDiffuseShader(const GraphicsDevicePtr& graphicsDevice);

    void update(const std::shared_ptr<Material>& material) const override;

protected:
};

} // namespace rfx