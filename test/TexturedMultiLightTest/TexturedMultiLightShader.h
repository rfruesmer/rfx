#pragma once

#include "TestMaterialShader.h"
#include "rfx/scene/Camera.h"
#include "rfx/scene/PointLight.h"
#include "rfx/scene/SpotLight.h"


namespace rfx {

class TexturedMultiLightShader : public TestMaterialShader
{
public:
    static const std::string ID;

    static const int MAX_LIGHTS = 4;

    explicit TexturedMultiLightShader(const GraphicsDevicePtr& graphicsDevice);

    [[nodiscard]] std::vector<std::byte> createDataFor(const MaterialPtr& material) const override;

    [[nodiscard]] const void* getData() const override;
    [[nodiscard]] uint32_t getDataSize() const override;

    void setCamera(CameraPtr camera);
    void setLight(int index, const PointLightPtr& light);
    void setLight(int index, const SpotLightPtr& light);
    void onViewMatrixChanged();

private:
    struct LightData {
        int type = Light::LightType::POINT;
        bool enabled = false;
        float pad1;
        float pad2;

        glm::vec3 position;
        float pad3;

        glm::vec3 color;
        float pad4;

        glm::vec3 direction;
        float pad5;

        float spotInnerConeAngle = 0.0f;
        float spotOuterConeAngle = 0.0f;
        float range = 0.0f;
        float pad6;
    };

    struct ShaderData {
        LightData lights[MAX_LIGHTS];
    };

    struct MaterialData {
        glm::vec4 baseColor;
        glm::vec3 specular;
        float shininess = 0.0f;
    };

    ShaderData data {};
    CameraPtr camera;
    PointLightPtr lights[MAX_LIGHTS];
};

} // namespace rfx