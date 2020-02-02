#pragma once

#include "test/TestApplication.h"

namespace rfx
{

class PerVertexSpotLightTest : public TestApplication
{
public:
    PerVertexSpotLightTest(handle_t instanceHandle);

    void initialize() override;

protected:
    PerVertexSpotLightTest(const std::filesystem::path& configurationPath, handle_t instanceHandle);

    void createEffectFactories() override;
};

}
