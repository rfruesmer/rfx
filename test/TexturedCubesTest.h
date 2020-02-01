#pragma once

#include "test/CubeTest.h"

namespace rfx
{

class TexturedCubesTest : public CubeTest
{
public:
    explicit TexturedCubesTest(handle_t instanceHandle);

    void createEffectFactories() override;
};

} // namespace rfx

