#pragma once

#include "TestMaterialShader.h"
#include "rfx/scene/PointLight.h"


namespace rfx {

class NormalMapShader : public TestMaterialShader
{
public:
    static const std::string ID;

    static const int MAX_LIGHTS = 4;

    explicit NormalMapShader(const GraphicsDevicePtr& graphicsDevice);

    [[nodiscard]] std::vector<std::byte> createDataFor(const MaterialPtr& material) const override;

    [[nodiscard]] const void* getData() const override;
    [[nodiscard]] uint32_t getDataSize() const override;

    void setLight(int index, const PointLightPtr& light);
    void enableNormalMap(bool state);

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

        float range = 0.0f;
        float pad5;
        float pad6;
        float pad7;
    };

    struct ShaderData {
        LightData lights[MAX_LIGHTS];
        int useNormalMap = 1;
    };

    struct MaterialData {
        glm::vec4 baseColor;
        glm::vec3 specular;
        float shininess = 0.0f;
    };

    ShaderData data {};
    PointLightPtr lights[MAX_LIGHTS];
};

} // namespace rfx