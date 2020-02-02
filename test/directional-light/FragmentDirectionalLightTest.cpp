#include "rfx/pch.h"
#include "test/directional-light/FragmentDirectionalLightTest.h"
#include "test/directional-light/FragmentDirectionalLightEffectFactory.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

FragmentDirectionalLightTest::FragmentDirectionalLightTest(handle_t instanceHandle)
    : TestApplication("assets/tests/fragment-directional-light/application-config.json", instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

FragmentDirectionalLightTest::FragmentDirectionalLightTest(const path& configurationPath, handle_t instanceHandle)
    : TestApplication(configurationPath, instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

void FragmentDirectionalLightTest::createEffectFactories()
{
    add(make_shared<FragmentDirectionalLightEffectFactory>());
}

// ---------------------------------------------------------------------------------------------------------------------
