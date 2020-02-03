#pragma once

#include "rfx/graphics/effect/Effect.h"
#include "rfx/graphics/GraphicsDevice.h"

namespace rfx
{

class VertexColorEffect : public Effect
{
public:
    static const std::string ID;

    explicit VertexColorEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        VkRenderPass renderPass,
        std::unique_ptr<ShaderProgram>& shaderProgram);

    [[nodiscard]] const std::string& getId() const override;

    void setModelViewProjMatrix(const glm::mat4& matrix) override;
    void updateUniformBuffer() override;

protected:
    void createUniformBuffers() override;
    void createDescriptorSetLayout() override;
    void createDescriptorPool() override;
    void updateDescriptorSets() override;

private:
    struct UniformData
    {
        glm::mat4 modelViewProjection = glm::mat4(1.0F);
    };

    UniformData uniformData;
};

}
