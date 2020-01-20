#include "rfx/pch.h"
#include "test/DirectionalVertexLightingTest.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

DirectionalVertexLightingTest::DirectionalVertexLightingTest(handle_t instanceHandle)
    : TestApplication("assets/tests/directional-vertex-lighting/application-config.json", instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

DirectionalVertexLightingTest::DirectionalVertexLightingTest(const path& configurationPath, handle_t instanceHandle)
    : TestApplication(configurationPath, instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

void DirectionalVertexLightingTest::initialize()
{
    TestApplication::initialize();

    initCommandPool();
    initRenderPass();
    initFrameBuffers();

    initScene();

    initCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

