#pragma once

#include "rfx/scene/MaterialShader.h"
#include "rfx/scene/MaterialShaderCache.h"

namespace rfx {

class MaterialShaderFactory
{
public:
    explicit MaterialShaderFactory(
        std::filesystem::path shadersDirectory,
        std::string defaultShaderId);

    void addAllocator(
        const std::string& shaderId,
        const std::function<MaterialShaderPtr()>& allocator);

    MaterialShaderPtr createShaderFor(const MaterialPtr& material);

    void clearCache();

private:
    size_t hash(const MaterialShaderPtr& shader, const MaterialPtr& material);

    std::filesystem::path shadersDirectory;
    std::string defaultShaderId;
    std::map<std::string, std::function<MaterialShaderPtr()>> allocatorMap;
    MaterialShaderCache shaderCache;
};

} // namespace rfx
