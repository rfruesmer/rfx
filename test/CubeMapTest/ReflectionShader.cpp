#include "rfx/pch.h"
#include "ReflectionShader.h"


using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

const string ReflectionShader::ID = "cubemap_reflection";

// ---------------------------------------------------------------------------------------------------------------------

ReflectionShader::ReflectionShader(const GraphicsDevicePtr& graphicsDevice)
    : TestMaterialShader(
        graphicsDevice,
        ID,
        ID,
        ID) {}

// ---------------------------------------------------------------------------------------------------------------------

vector<std::byte> ReflectionShader::createDataFor(const MaterialPtr& material) const
{
    return std::vector<std::byte>();
}

// ---------------------------------------------------------------------------------------------------------------------

const void* ReflectionShader::getData() const
{
    return nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ReflectionShader::getDataSize() const
{
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------



