#pragma once

#include "rfx/scene/Mesh.h"
#include "rfx/graphics/GraphicsDevice.h"

namespace rfx
{
class VertexFormat;

class ModelLoader
{
public:
    explicit ModelLoader(const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    std::shared_ptr<Mesh> load(const std::filesystem::path& modelPath, const VertexFormat& vertexFormat);

private:
    std::shared_ptr<GraphicsDevice> graphicsDevice;
};
    
}
