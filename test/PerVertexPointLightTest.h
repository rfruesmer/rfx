#pragma once

#include "test/TestApplication.h"

namespace rfx
{

class PerVertexPointLightTest : public TestApplication
{
public:
    PerVertexPointLightTest(handle_t instanceHandle);

    void initialize() override;

protected:
    PerVertexPointLightTest(const std::filesystem::path& configurationPath, handle_t instanceHandle);

    void createEffectFactories() override;
};

}
