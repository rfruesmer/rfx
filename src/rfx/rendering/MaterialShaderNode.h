#pragma once

#include "rfx/rendering/MaterialNode.h"
#include "rfx/scene/MaterialShader.h"
#include "rfx/scene/Model.h"

namespace rfx {

class MaterialShaderNode
{
public:
    MaterialShaderNode(
        MaterialShaderPtr shader,
        const std::vector<MaterialPtr>& materials,
        const ModelPtr& model);

    [[nodiscard]] const MaterialShaderPtr& getMaterialShader() const;

    [[nodiscard]] const std::vector<MaterialNode>& getChildNodes() const;

private:
    void add(const std::vector<MaterialPtr>& materials, const ModelPtr& model);

    MaterialShaderPtr materialShader;
    std::vector<MaterialNode> childNodes;
};

} // namespace rfx
