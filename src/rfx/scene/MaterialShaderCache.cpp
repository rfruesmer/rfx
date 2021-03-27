#include "rfx/pch.h"
#include "rfx/scene/MaterialShaderCache.h"


using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

void MaterialShaderCache::add(size_t hash, MaterialShaderPtr shader)
{
    RFX_CHECK_ARGUMENT(!cache.contains(hash));

    cache[hash] = shader;
}

// ---------------------------------------------------------------------------------------------------------------------

MaterialShaderPtr MaterialShaderCache::get(size_t hash) const
{
    const auto it = cache.find(hash);

    return it != cache.end()
        ? it->second
        : nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

void MaterialShaderCache::clear()
{
    cache.clear();
}

// ---------------------------------------------------------------------------------------------------------------------
