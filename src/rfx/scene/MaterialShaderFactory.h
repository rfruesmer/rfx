#pragma once

#include "rfx/scene/MaterialShader.h"
#include "rfx/scene/MaterialShaderCache.h"

namespace rfx {

class MaterialShaderFactory
{
public:
    MaterialShaderFactory(
        GraphicsDevicePtr graphicsDevice,
        VkDescriptorPool descriptorPool,
        std::filesystem::path shadersDirectory,
        std::string defaultShaderId);

    void addAllocator(
        const std::string& shaderId,
        const std::function<MaterialShaderPtr()>& allocator);

    MaterialShaderPtr createShaderFor(const MaterialPtr& material);

    void clearCache();

private:
    MaterialShaderPtr getCachedShaderFor(const MaterialPtr& material);
    size_t hash(const MaterialPtr& material);
    static size_t hash(const MaterialShaderPtr& shader, const MaterialPtr& material);
    MaterialShaderPtr createShader(const MaterialPtr& material);
    void addToCache(const MaterialShaderPtr& shader, const MaterialPtr& material);
    VkDescriptorSetLayout createMaterialDescriptorSetLayoutFor(const MaterialPtr& material);
    void initMaterialUniformBuffer(const MaterialPtr& material, const MaterialShaderPtr& shader);
    BufferPtr createAndBindUniformBuffer(VkDeviceSize bufferSize);
    void initMaterialDescriptorSetLayout(const MaterialPtr& material, const MaterialShaderPtr& shader);
    VkDescriptorSet createMaterialDescriptorSetFor(const MaterialPtr& material, VkDescriptorSetLayout descriptorSetLayout);
    static VkWriteDescriptorSet buildWriteDescriptorSet(
        VkDescriptorSet descriptorSet,
        uint32_t binding,
        const VkDescriptorImageInfo* descriptorImageInfo);
    static VkWriteDescriptorSet buildWriteDescriptorSet(
        VkDescriptorSet descriptorSet,
        uint32_t binding,
        const VkDescriptorBufferInfo* descriptorBufferInfo);

    GraphicsDevicePtr graphicsDevice;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::filesystem::path shadersDirectory;
    std::string defaultShaderId;
    std::map<std::string, std::function<MaterialShaderPtr()>> allocatorMap;
    MaterialShaderCache shaderCache;
};

} // namespace rfx
