#pragma once

#include "test/TestApplication.h"

namespace rfx
{

class CubeTest : public TestApplication
{
public:
    explicit CubeTest(handle_t instanceHandle);

protected:
    CubeTest(const std::filesystem::path& configurationPath, handle_t instanceHandle);

    void createEffectFactories() override;
};

} // namespace rfx

