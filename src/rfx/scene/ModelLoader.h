#pragma once

#include "rfx/scene/Model.h"
#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/graphics/Image.h"
#include "rfx/graphics/Texture2D.h"


namespace rfx {

class ModelLoader
{
public:
    explicit ModelLoader(
        std::shared_ptr<GraphicsDevice> graphicsDevice);

    const std::shared_ptr<Model>& load(const std::filesystem::path& scenePath);


private:
    class ModelLoaderImpl;

    std::unique_ptr<ModelLoaderImpl, void(*)(ModelLoaderImpl*)> pimpl_;
};

} // namespace rfx::test