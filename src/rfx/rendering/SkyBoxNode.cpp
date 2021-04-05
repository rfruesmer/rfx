#include "rfx/pch.h"
#include "rfx/rendering/SkyBoxNode.h"


using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

SkyBoxNode::SkyBoxNode(SkyBoxPtr skyBox)
    : skyBox(move(skyBox)) {}

// ---------------------------------------------------------------------------------------------------------------------

void SkyBoxNode::record(const CommandBufferPtr& commandBuffer) const
{
    const ModelPtr& skyBoxModel = skyBox->getModel();
    const vector<shared_ptr<VertexBuffer>> vertexBuffers = { skyBoxModel->getVertexBuffer() };

    commandBuffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, skyBox->getPipeline());
    commandBuffer->bindDescriptorSet(
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        skyBox->getPipelineLayout(),
        0,
        skyBox->getDescriptorSet());
    commandBuffer->bindVertexBuffers(vertexBuffers);
    commandBuffer->bindIndexBuffer(skyBoxModel->getIndexBuffer());
    commandBuffer->drawIndexed(skyBoxModel->getIndexBuffer()->getIndexCount());
}

// ---------------------------------------------------------------------------------------------------------------------
