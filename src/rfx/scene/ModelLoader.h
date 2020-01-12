#pragma once

#include "rfx/scene/Mesh.h"
#include "rfx/graphics/GraphicsDevice.h"

namespace rfx
{
class VertexFormat;

class ModelLoader
{
public:
    static const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

    explicit ModelLoader(const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    std::shared_ptr<Mesh> load(const std::filesystem::path& modelPath, const VertexFormat& vertexFormat);

private:
    void loadInternal(const std::filesystem::path& modelPath, const VertexFormat& vertexFormat);
    void createBuffers(const VertexFormat& vertexFormat);
    void copyBuffers();

    std::shared_ptr<GraphicsDevice> graphicsDevice;

    uint32_t vertexCount = 0;
    uint32_t vertexBufferSize = 0;
    std::vector<float> vertexData;
    std::shared_ptr<Buffer> stagingVertexBuffer;
    std::shared_ptr<VertexBuffer> vertexBuffer;

    uint32_t indexCount = 0;
    std::vector<uint32_t> indexData;
    std::shared_ptr<Buffer> stagingIndexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;
};
    
}
