#include "rfx/pch.h"
#include "rfx/rendering/RenderGraphNode.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

bool RenderGraphNode::isEnabled() const
{
    return enabled;
}

// ---------------------------------------------------------------------------------------------------------------------

void RenderGraphNode::setEnabled(bool enabled)
{
    RenderGraphNode::enabled = enabled;
}

// ---------------------------------------------------------------------------------------------------------------------
