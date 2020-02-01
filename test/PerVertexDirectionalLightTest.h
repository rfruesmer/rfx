#pragma once

#include "test/TestApplication.h"


namespace rfx
{

class PerVertexDirectionalLightTest : public TestApplication
{
public:
    PerVertexDirectionalLightTest(handle_t instanceHandle);

    void initialize() override;

protected:
    PerVertexDirectionalLightTest(const std::filesystem::path& configurationPath, handle_t instanceHandle);

    void createEffectFactories() override;
};

}
