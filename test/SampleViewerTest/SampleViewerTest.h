#pragma once

#include "TestApplication.h"

namespace rfx::test {

class SampleViewerTest : public TestApplication
{
protected:
    void initGraphics() override;
    void initShaderFactory(MaterialShaderFactory& shaderFactory) override;

private:
    void buildRenderGraph();

};

}