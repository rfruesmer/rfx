#include "rfx/pch.h"
#include "test/CubeTest.h"
#include "rfx/graphics/effect/VertexColorEffectFactory.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

CubeTest::CubeTest(handle_t instanceHandle)
    : TestApplication("assets/tests/cube/application-config.json", instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

CubeTest::CubeTest(const path& configurationPath, handle_t instanceHandle)
    : TestApplication(configurationPath, instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

void CubeTest::createEffectFactories()
{
    add(make_shared<VertexColorEffectFactory>());
}

// ---------------------------------------------------------------------------------------------------------------------
