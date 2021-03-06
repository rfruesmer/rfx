#pragma once

#include "TestEffect.h"
#include "rfx/scene/Scene.h"
#include "rfx/scene/PointLight.h"
#include "rfx/scene/SpotLight.h"


namespace rfx {

class TexturedMultiLightEffect : public TestEffect
{
public:
    static const int MAX_LIGHTS = 4;

    static inline const VertexFormat VERTEX_FORMAT {
        VertexFormat::COORDINATES | VertexFormat::NORMALS | VertexFormat::TEXCOORDS
    };

    TexturedMultiLightEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<Scene>& scene);

    void createDescriptorPool() override;
    void createDescriptorSetLayouts() override;
    void createDescriptorSets() override;

    void setProjectionMatrix(const glm::mat4& projection);
    void setViewMatrix(const glm::mat4& viewMatrix);
    void setLight(int index, const std::shared_ptr<PointLight>& light);
    void setLight(int index, const std::shared_ptr<SpotLight>& light);

    [[nodiscard]] size_t getSceneDataSize() const override;
    void updateSceneDataBuffer();

    [[nodiscard]] VertexFormat getVertexFormat() const override { return VERTEX_FORMAT; };
    [[nodiscard]] std::string getVertexShaderFileName() const override;
    [[nodiscard]] std::string getFragmentShaderFileName() const override;

    [[nodiscard]] const std::vector<VkDescriptorSet>& getDescriptorSets();
    [[nodiscard]] std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts() override;

    void cleanupSwapChain() override;

private:
    struct LightData {
        glm::vec3 position;          // light position in eye coords
        float pad1;
        glm::vec3 color;
        float pad2;
        glm::vec3 direction;
        float pad3;
        float exponent;
        float cutoff;
        int type = Light::LightType::POINT;
        bool enabled = false;
    };

    struct SceneData {
        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;
        LightData lights[MAX_LIGHTS];
    };

    VkDescriptorSetLayout descSetLayout_ = nullptr;
    std::vector<VkDescriptorSet> descSets_;

    SceneData sceneData_ {};
    std::shared_ptr<PointLight> lights_[MAX_LIGHTS];
};

} // namespace rfx