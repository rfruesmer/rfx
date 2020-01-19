#pragma once

#include "test/TestApplication.h"
#include "rfx/graphics/VertexColorEffect.h"

namespace rfx
{

class CubeTest : public TestApplication
{
public:
    CubeTest(handle_t instanceHandle);

    void initialize() override;

protected:
    CubeTest(const std::filesystem::path& configurationPath, handle_t instanceHandle);
};

} // namespace rfx

