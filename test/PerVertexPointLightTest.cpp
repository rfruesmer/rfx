#include "rfx/pch.h"
#include "test/PerVertexPointLightTest.h"
#include "rfx/graphics/effect/PointLightEffectFactory.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

PerVertexPointLightTest::PerVertexPointLightTest(handle_t instanceHandle)
    : TestApplication("assets/tests/vertex-point-light/application-config.json", instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

PerVertexPointLightTest::PerVertexPointLightTest(const path& configurationPath, handle_t instanceHandle)
    : TestApplication(configurationPath, instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

void PerVertexPointLightTest::createEffectFactories()
{
    add(make_shared<PointLightEffectFactory>());
}

// ---------------------------------------------------------------------------------------------------------------------
