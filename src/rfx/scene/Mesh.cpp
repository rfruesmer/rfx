#include "rfx/pch.h"
#include "rfx/scene/Mesh.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Mesh::Mesh(const shared_ptr<GraphicsDevice>& graphicsDevice,
           const shared_ptr<VertexBuffer>& vertexBuffer, 
           const shared_ptr<IndexBuffer>& indexBuffer,
           const std::shared_ptr<Effect>& effect)
    : graphicsDevice(graphicsDevice),
      vertexBuffer(vertexBuffer),
      indexBuffer(indexBuffer),
      effect(effect) {}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<VertexBuffer>& Mesh::getVertexBuffer() const
{
    return vertexBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<IndexBuffer>& Mesh::getIndexBuffer() const
{
    return indexBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Effect>& Mesh::getEffect() const
{
    return effect;
}

// ---------------------------------------------------------------------------------------------------------------------
