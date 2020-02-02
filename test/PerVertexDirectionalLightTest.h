#pragma once

#include "test/TestApplication.h"


namespace rfx
{

class PerVertexDirectionalLightTest : public TestApplication
{
public:
    explicit PerVertexDirectionalLightTest(handle_t instanceHandle);

protected:
    PerVertexDirectionalLightTest(const std::filesystem::path& configurationPath, handle_t instanceHandle);

    void createEffectFactories() override;
};

}
