#pragma once


#include "rfx/rendering/RenderGraphNode.h"
#include "rfx/scene/SkyBox.h"


namespace rfx {

class SkyBoxNode : public RenderGraphNode
{
public:
    explicit SkyBoxNode(SkyBoxPtr skyBox);

    void record(const CommandBufferPtr& commandBuffer) const override;

private:
    SkyBoxPtr skyBox;
};

using SkyBoxNodePtr = std::shared_ptr<SkyBoxNode>;

} // namespace rfx
