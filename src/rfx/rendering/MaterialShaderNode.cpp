#include "rfx/pch.h"
#include "rfx/rendering/MaterialShaderNode.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

MaterialShaderNode::MaterialShaderNode(
    MaterialShaderPtr shader,
    const vector<MaterialPtr>& materials,
    const ModelPtr& model)
        : materialShader(move(shader))
{
    add(materials, model);
}

// ---------------------------------------------------------------------------------------------------------------------

void MaterialShaderNode::add(const vector<MaterialPtr>& materials, const ModelPtr& model)
{
    for (const auto& material : materials) {
        MaterialNode childNode(material, model);
        childNodes.push_back(childNode);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const MaterialShaderPtr& MaterialShaderNode::getMaterialShader() const
{
    return materialShader;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<MaterialNode>& MaterialShaderNode::getChildNodes() const
{
    return childNodes;
}

// ---------------------------------------------------------------------------------------------------------------------
