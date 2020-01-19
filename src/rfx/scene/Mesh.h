#pragma once

#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/graphics/effect/Effect.h"

namespace rfx
{

class Mesh
{
public:
    Mesh(const std::shared_ptr<GraphicsDevice>& graphicsDevice,
         const std::shared_ptr<VertexBuffer>& vertexBuffer, 
         const std::shared_ptr<IndexBuffer>& indexBuffer,
         const std::shared_ptr<Effect>& effect);

    const std::shared_ptr<VertexBuffer>& getVertexBuffer() const;
    const std::shared_ptr<IndexBuffer>& getIndexBuffer() const;
    const std::shared_ptr<Effect>& getEffect() const;

private:
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<Effect> effect;
};
    
}
