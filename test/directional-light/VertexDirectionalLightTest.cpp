#include "rfx/pch.h"
#include "test/directional-light/VertexDirectionalLightTest.h"
#include "test/directional-light/VertexDirectionalLightEffectFactory.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

VertexDirectionalLightTest::VertexDirectionalLightTest(handle_t instanceHandle)
    : TestApplication("assets/tests/vertex-directional-light/application-config.json", instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

VertexDirectionalLightTest::VertexDirectionalLightTest(const path& configurationPath, handle_t instanceHandle)
    : TestApplication(configurationPath, instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDirectionalLightTest::createEffectFactories()
{
    add(make_shared<VertexDirectionalLightEffectFactory>());
}

// ---------------------------------------------------------------------------------------------------------------------
