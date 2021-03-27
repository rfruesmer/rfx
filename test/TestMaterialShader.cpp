#include "rfx/pch.h"
#include "TestMaterialShader.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

TestMaterialShader::TestMaterialShader(
    GraphicsDevicePtr graphicsDevice,
    std::string id,
    std::string vertexShaderId,
    std::string fragmentShaderId)
        : MaterialShader(
            move(graphicsDevice),
            move(id),
            move(vertexShaderId),
            move(fragmentShaderId)) {}

// ---------------------------------------------------------------------------------------------------------------------
