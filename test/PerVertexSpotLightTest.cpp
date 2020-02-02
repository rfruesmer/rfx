#include "rfx/pch.h"
#include "test/PerVertexSpotLightTest.h"
#include "rfx/graphics/effect/SpotLightEffectFactory.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

PerVertexSpotLightTest::PerVertexSpotLightTest(handle_t instanceHandle)
    : TestApplication("assets/tests/vertex-spot-light/application-config.json", instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

PerVertexSpotLightTest::PerVertexSpotLightTest(const path& configurationPath, handle_t instanceHandle)
    : TestApplication(configurationPath, instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

void PerVertexSpotLightTest::initialize()
{
    TestApplication::initialize();

    initCommandPool();
    initRenderPass();
    initFrameBuffers();

    initScene();

    initCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void PerVertexSpotLightTest::createEffectFactories()
{
    add(make_shared<SpotLightEffectFactory>());
}

// ---------------------------------------------------------------------------------------------------------------------
