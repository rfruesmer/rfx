#pragma once

#include "rfx/scene/MaterialShader.h"


namespace rfx {

class MaterialShaderCache
{
public:
    void add(size_t hash, MaterialShaderPtr shader);

    [[nodiscard]] MaterialShaderPtr get(size_t hash) const;

    void clear();

private:
    std::unordered_map<size_t, MaterialShaderPtr> cache;
};

} // namespace rfx

