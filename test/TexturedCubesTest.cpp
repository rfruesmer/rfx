#include "rfx/pch.h"
#include "test/TexturedCubesTest.h"
#include "rfx/graphics/effect/Texture2DEffectFactory.h"


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
