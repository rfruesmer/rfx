#include "rfx/pch.h"
#include "SkyBoxShader.h"


using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

const string SkyBoxShader::ID = "skybox";

// ---------------------------------------------------------------------------------------------------------------------

SkyBoxShader::SkyBoxShader(GraphicsDevicePtr& graphicsDevice)
    : TestMaterialShader(
        graphicsDevice,
        ID,
        ID,
        ID) {}

// ---------------------------------------------------------------------------------------------------------------------

vector<std::byte> SkyBoxShader::createDataFor(const MaterialPtr& material) const
{
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

const void* SkyBoxShader::getData() const
{
    return nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t SkyBoxShader::getDataSize() const
{
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
