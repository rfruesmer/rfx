#include "rfx/pch.h"
#include "test/VertexSpotLightTest.h"
#include "rfx/graphics/effect/VertexSpotLightEffectFactory.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

VertexSpotLightTest::VertexSpotLightTest(handle_t instanceHandle)
    : TestApplication("assets/tests/vertex-spot-light/application-config.json", instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

VertexSpotLightTest::VertexSpotLightTest(const path& configurationPath, handle_t instanceHandle)
    : TestApplication(configurationPath, instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

void VertexSpotLightTest::createEffectFactories()
{
    add(make_shared<VertexSpotLightEffectFactory>());
}

// ---------------------------------------------------------------------------------------------------------------------
