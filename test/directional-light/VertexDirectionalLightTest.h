#pragma once

#include "test/TestApplication.h"


namespace rfx
{

class VertexDirectionalLightTest final : public TestApplication
{
public:
    explicit VertexDirectionalLightTest(handle_t instanceHandle);

protected:
    VertexDirectionalLightTest(const std::filesystem::path& configurationPath, handle_t instanceHandle);

    void createEffectFactories() override;
};

}
