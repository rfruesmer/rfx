#pragma once

#include "test/TestApplication.h"

namespace rfx
{

class DirectionalVertexLightingTest : public TestApplication
{
public:
    DirectionalVertexLightingTest(handle_t instanceHandle);

    void initialize() override;

protected:
    DirectionalVertexLightingTest(const std::filesystem::path& configurationPath, handle_t instanceHandle);
};

}
