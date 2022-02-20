#pragma once

#include "rfx/scene/Scene.h"


namespace rfx {

class SceneLoader
{
public:
    explicit SceneLoader(GraphicsDevicePtr graphicsDevice);

    ScenePtr load(const std::filesystem::path& path);

private:
    GraphicsDevicePtr graphicsDevice;
};


} // namespace rfx
