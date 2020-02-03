#pragma once

#include "rfx/graphics/effect/Effect.h"



namespace rfx
{

class VertexPointLightEffect : public Effect
{
public:
    static const std::string ID;

    explicit VertexPointLightEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        VkRenderPass renderPass,
        std::unique_ptr<ShaderProgram>& shaderProgram);

    const std::string& getId() const override;

    void updateFrom(const std::vector<std::shared_ptr<Light>>& lights) override;
    void updateFrom(const std::shared_ptr<Material>& material) override;
    void updateFrom(const std::shared_ptr<Camera>& camera) override;

    void setModelViewProjMatrix(const glm::mat4& matrix) override;
    void updateUniformBuffer() override;

protected:
    void createUniformBuffers() override;
    void createDescriptorSetLayout() override;
    void createDescriptorPool() override;
    void updateDescriptorSets() override;

    struct UniformData
    {
        glm::mat4 modelView;
        glm::mat4 modelViewProjection;
        Light::Data lightData;
        Material::Data materialData;
    };

    UniformData uniformData;
};

}
