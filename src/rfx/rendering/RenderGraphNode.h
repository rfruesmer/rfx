#pragma once

#include "rfx/graphics/CommandBuffer.h"


namespace rfx {

class RenderGraphNode
{
public:
    virtual ~RenderGraphNode() = default;

    virtual void record(const CommandBufferPtr& commandBuffer) const = 0;

    bool isEnabled() const;

    void setEnabled(bool enabled);

private:
    bool enabled = true;
};

using RenderGraphNodePtr = std::shared_ptr<RenderGraphNode>;

} // namespace rfx