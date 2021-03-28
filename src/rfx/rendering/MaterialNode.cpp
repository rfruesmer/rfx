#include "rfx/pch.h"
#include "rfx/rendering/MaterialNode.h"

#include <utility>

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

MaterialNode::MaterialNode(const MaterialPtr& material, const ModelPtr& model)
    : material(material)
{
    add(material, model);
}

// ---------------------------------------------------------------------------------------------------------------------

void MaterialNode::add(const MaterialPtr& material, const ModelPtr& model)
{
    for (const auto& mesh : model->getMeshes())
    {
        MeshNode childNode(mesh, material);
        if (!childNode.isEmpty()) {
            childNodes.push_back(childNode);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const MaterialPtr& MaterialNode::getMaterial() const
{
    return material;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<MeshNode>& MaterialNode::getChildNodes() const
{
    return childNodes;
}

// ---------------------------------------------------------------------------------------------------------------------
