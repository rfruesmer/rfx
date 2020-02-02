#include "rfx/pch.h"
#include "test/point-light/VertexPointLightTest.h"
#include "test/point-light/VertexPointLightEffectFactory.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

VertexPointLightTest::VertexPointLightTest(handle_t instanceHandle)
    : TestApplication("assets/tests/vertex-point-light/application-config.json", instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

VertexPointLightTest::VertexPointLightTest(const path& configurationPath, handle_t instanceHandle)
    : TestApplication(configurationPath, instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

void VertexPointLightTest::createEffectFactories()
{
    add(make_shared<VertexPointLightEffectFactory>());
}

// ---------------------------------------------------------------------------------------------------------------------
