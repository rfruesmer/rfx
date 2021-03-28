#pragma once

#include "rfx/rendering/MeshNode.h"
#include "rfx/scene/Material.h"
#include "rfx/scene/Model.h"


namespace rfx {

class MaterialNode
{
public:
    MaterialNode(const MaterialPtr& material, const ModelPtr& model);

    [[nodiscard]] const MaterialPtr& getMaterial() const;
    [[nodiscard]] const std::vector<MeshNode>& getChildNodes() const;

private:
    void add(const MaterialPtr& material, const ModelPtr& model);

    MaterialPtr material;
    std::vector<MeshNode> childNodes;
};

} // namespace rfx
