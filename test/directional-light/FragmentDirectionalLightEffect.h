#pragma once

#include "rfx/graphics/effect/Effect.h"



namespace rfx
{

class FragmentDirectionalLightEffect final : public Effect
{
public:
    static const std::string ID;

    explicit FragmentDirectionalLightEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        VkRenderPass renderPass,
        std::unique_ptr<ShaderProgram>& shaderProgram);

    const std::string& getId() const override;

    void updateFrom(const std::vector<std::shared_ptr<Light>>& lights) override;
    void updateFrom(const std::shared_ptr<Material>& material) override;
    void updateFrom(const std::shared_ptr<Camera>& camera) override;

    void setModelViewProjMatrix(const glm::mat4& matrix) override;
    void updateUniformBuffer() override;

private:
    struct VertexShaderConstants
    {
        glm::mat4 modelView = glm::mat4(1.0F);
        glm::mat4 modelViewProjection = glm::mat4(1.0F);
    };

    struct FragmentShaderConstants
    {
        glm::mat4 modelView = glm::mat4(1.0F);
        Light::Data lightData;
        Material::Data materialData;
    };

    void initDescriptorSetLayout();
    void initDescriptorSet();

    VertexShaderConstants vertexShaderConstants;
    FragmentShaderConstants fragmentShaderConstants;
};
    
}
