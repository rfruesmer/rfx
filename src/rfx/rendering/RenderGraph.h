#pragma once

#include "rfx/scene/Model.h"
#include "rfx/scene/MaterialShader.h"
#include "rfx/rendering/MaterialShaderNode.h"


namespace rfx {

class RenderGraph
{
public:
    void add(
        const ModelPtr& model,
        const std::unordered_map<MaterialShaderPtr, std::vector<MaterialPtr>>& materialShaderMap);

    [[nodiscard]] const std::vector<MaterialShaderNode>& getChildNodes() const;

    void clear();

private:
    void add(
        const MaterialShaderPtr& shader,
        const std::vector<MaterialPtr>& materials,
        const ModelPtr& model);


    std::vector<MaterialShaderNode> childNodes;
};

} // namespace rfx
