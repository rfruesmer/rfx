#include "rfx/pch.h"
#include "SampleViewerTest.h"
#include "rfx/common/Logger.h"

using namespace rfx;
using namespace rfx::test;
using namespace glm;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    try {
        auto theApp = make_shared<SampleViewerTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::initGraphics()
{
    TestApplication::initGraphics();

    initGraphicsResources();
    buildRenderGraph();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::buildRenderGraph()
{
//    renderGraph = make_shared<RenderGraph>(graphicsDevice, sceneDescriptorSet_);
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::initShaderFactory(MaterialShaderFactory& shaderFactory)
{
//    shaderFactory.addAllocator(TexturedPBRShader::ID,
//        [this] { return make_shared<TexturedPBRShader>(graphicsDevice); });
}

// ---------------------------------------------------------------------------------------------------------------------
