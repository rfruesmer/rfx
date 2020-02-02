#include "rfx/pch.h"
#include "test/texture-mapping/TexturedCubesTest.h"
#include "test/texture-mapping/Texture2DEffectFactory.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

TexturedCubesTest::TexturedCubesTest(handle_t instanceHandle)
    : CubeTest("assets/tests/textured-cubes/application-config.json", instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedCubesTest::createEffectFactories()
{
    add(make_shared<Texture2DEffectFactory>());
}

// ---------------------------------------------------------------------------------------------------------------------
