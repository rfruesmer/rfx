#pragma once

#include "rfx/scene/MaterialShader.h"
#include "rfx/scene/Model.h"


namespace rfx {

class TestMaterialShader : public MaterialShader
{
public:
    TestMaterialShader(
        GraphicsDevicePtr graphicsDevice,
        std::string vertexShaderId,
        std::string fragmentShaderId);
};

} // namespace rfx