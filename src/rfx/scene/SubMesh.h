#pragma once

#include "rfx/scene/Material.h"

namespace rfx {

struct SubMesh
{
public:
    SubMesh(
        uint32_t firstIndex,
        uint32_t indexCount,
        MaterialPtr material);

    [[nodiscard]] uint32_t getFirstIndex() const;
    [[nodiscard]] uint32_t getIndexCount() const;

    void setMaterial(const MaterialPtr& material);
    [[nodiscard]] const MaterialPtr& getMaterial() const;

private:
    uint32_t firstIndex = 0;
    uint32_t indexCount = 0;
    MaterialPtr material;
};

} // namespace rfx