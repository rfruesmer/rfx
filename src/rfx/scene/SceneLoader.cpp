#include "rfx/pch.h"
#include "rfx/scene/SceneLoader.h"
#include "rfx/scene/GltfSceneImporter.h"

using namespace rfx;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

SceneLoader::SceneLoader(GraphicsDevicePtr graphicsDevice)
    : graphicsDevice(move(graphicsDevice)) {}

// ---------------------------------------------------------------------------------------------------------------------

ScenePtr SceneLoader::load(const path& path)
{
    const string extension = path.extension().string();

    if (extension == ".rfx") {
        // TODO: RfxSceneLoader
        RFX_THROW_NOT_IMPLEMENTED();
    }
    else if (extension == ".gltf" || extension == ".glb") {
        GltfSceneImporter gltfSceneImporter(graphicsDevice);
        return gltfSceneImporter.import(path);
    }
    else {
        // TODO: AssimpSceneImporter
        RFX_THROW_NOT_IMPLEMENTED();
    }

    return nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------
