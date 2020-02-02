#pragma once

#include "test/TestApplication.h"

namespace rfx
{

class VertexPointLightTest final : public TestApplication
{
public:
    explicit VertexPointLightTest(handle_t instanceHandle);

protected:
    VertexPointLightTest(const std::filesystem::path& configurationPath, handle_t instanceHandle);

    void createEffectFactories() override;
};

}
