#pragma once

#include "test/TestApplication.h"

namespace rfx
{

class PerVertexSpotLightTest : public TestApplication
{
public:
    explicit PerVertexSpotLightTest(handle_t instanceHandle);

protected:
    PerVertexSpotLightTest(const std::filesystem::path& configurationPath, handle_t instanceHandle);

    void createEffectFactories() override;
};

}
