#include "rfx/pch.h"
#include "rfx/rendering/RenderGraph.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

void RenderGraph::add(
    const ModelPtr& model,
    const unordered_map<MaterialShaderPtr, vector<MaterialPtr>>& materialShaderMap)
{
    for (const auto& [shader, materials] : materialShaderMap) {
        add(shader, materials, model);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void RenderGraph::add(
    const MaterialShaderPtr& shader,
    const vector<MaterialPtr>& materials,
    const ModelPtr& model)
{
    MaterialShaderNode childNode(shader, materials, model);
    childNodes.push_back(childNode);
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<MaterialShaderNode>& RenderGraph::getChildNodes() const
{
    return childNodes;
}

// ---------------------------------------------------------------------------------------------------------------------

void RenderGraph::clear()
{
    childNodes.clear();
}

// ---------------------------------------------------------------------------------------------------------------------
