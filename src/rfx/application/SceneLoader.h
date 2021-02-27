#pragma once

#include "rfx/scene/Scene.h"
#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/graphics/Image.h"
#include "rfx/graphics/Texture2D.h"


namespace rfx {

class SceneLoader
{
public:
    explicit SceneLoader(std::shared_ptr<GraphicsDevice> graphicsDevice);

    const std::shared_ptr<Scene>& load(const std::filesystem::path& scenePath);

private:
    class SceneLoaderImpl;

    std::unique_ptr<SceneLoaderImpl, void(*)(SceneLoaderImpl*)> pimpl_;
};

} // namespace rfx::test