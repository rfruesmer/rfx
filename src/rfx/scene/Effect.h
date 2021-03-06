#pragma once

#include "rfx/graphics/VertexFormat.h"

namespace rfx {

class Effect
{
public:
    virtual ~Effect() = default;

    virtual void createUniformBuffers() = 0;
    virtual void createDescriptorPools() = 0;
    virtual void createDescriptorSetLayouts() = 0;
    virtual void createDescriptorSets() = 0;

    virtual void cleanupSwapChain() = 0;

    [[nodiscard]] virtual VertexFormat getVertexFormat() const = 0;
    [[nodiscard]] virtual std::string getVertexShaderFileName() const = 0;
    [[nodiscard]] virtual std::string getFragmentShaderFileName() const = 0;

    [[nodiscard]] virtual std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts() = 0;
    [[nodiscard]] virtual VkDescriptorSet getSceneDescriptorSet() const = 0;
    [[nodiscard]] virtual const std::vector<VkDescriptorSet>& getMaterialDescriptorSets() const = 0;
    [[nodiscard]] virtual const std::vector<VkDescriptorSet>& getMeshDescriptorSets() const = 0;

protected:
    Effect() = default;
};

} // namespace rfx