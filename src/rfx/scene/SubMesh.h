#pragma once

#include "rfx/scene/Material.h"

namespace rfx {

struct SubMesh
{
    uint32_t firstIndex;
    uint32_t indexCount;
    std::shared_ptr<Material> material;
};

} // namespace rfx