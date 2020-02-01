#pragma once

#include "test/TestApplication.h"

namespace rfx
{

class CubeTest : public TestApplication
{
public:
    CubeTest(handle_t instanceHandle);

    void initialize() override;

protected:
    CubeTest(const std::filesystem::path& configurationPath, handle_t instanceHandle);

    void createEffectFactories() override;
};

} // namespace rfx

