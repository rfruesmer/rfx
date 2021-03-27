#pragma once

#include "rfx/scene/MaterialShader.h"
#include "rfx/scene/MaterialShaderCache.h"

namespace rfx {

class MaterialShaderFactory
{
public:
    explicit MaterialShaderFactory(
        GraphicsDevicePtr graphicsDevice,
        std::filesystem::path shadersDirectory,
        std::string defaultShaderId);

    void addAllocator(
        const std::string& shaderId,
        const std::function<MaterialShaderPtr()>& allocator);

    MaterialShaderPtr createShaderFor(const MaterialPtr& material);

    void clearCache();

private:
    static size_t hash(const MaterialShaderPtr& shader, const MaterialPtr& material);
    VkDescriptorSetLayout createMaterialDescriptorSetLayoutFor(const MaterialPtr& material);

    GraphicsDevicePtr graphicsDevice;
    std::filesystem::path shadersDirectory;
    std::string defaultShaderId;
    std::map<std::string, std::function<MaterialShaderPtr()>> allocatorMap;
    MaterialShaderCache shaderCache;
};

} // namespace rfx
