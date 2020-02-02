#pragma once

#include "test/TestApplication.h"

namespace rfx
{

class VertexSpotLightTest : public TestApplication
{
public:
    explicit VertexSpotLightTest(handle_t instanceHandle);

protected:
    VertexSpotLightTest(const std::filesystem::path& configurationPath, handle_t instanceHandle);

    void createEffectFactories() override;
};

}
