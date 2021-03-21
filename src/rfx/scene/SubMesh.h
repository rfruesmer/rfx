#pragma once

namespace rfx {

struct SubMesh
{
    uint32_t firstIndex;
    uint32_t indexCount;
    uint32_t materialIndex; // TODO: reference material directly
};

} // namespace rfx