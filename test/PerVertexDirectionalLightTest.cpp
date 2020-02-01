#include "rfx/pch.h"
#include "test/PerVertexDirectionalLightTest.h"
#include "rfx/graphics/effect/DirectionalLightEffectFactory.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

PerVertexDirectionalLightTest::PerVertexDirectionalLightTest(handle_t instanceHandle)
    : TestApplication("assets/tests/vertex-directional-light/application-config.json", instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

PerVertexDirectionalLightTest::PerVertexDirectionalLightTest(const path& configurationPath, handle_t instanceHandle)
    : TestApplication(configurationPath, instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

void PerVertexDirectionalLightTest::initialize()
{
    TestApplication::initialize();

    initCommandPool();
    initRenderPass();
    initFrameBuffers();

    initScene();

    initCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void PerVertexDirectionalLightTest::createEffectFactories()
{
    add(make_shared<DirectionalLightEffectFactory>());
}

// ---------------------------------------------------------------------------------------------------------------------
