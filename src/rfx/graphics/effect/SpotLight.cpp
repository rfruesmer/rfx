#include "rfx/pch.h"
#include "rfx/graphics/effect/SpotLight.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

SpotLight::SpotLight(const string& id)
    : Light(id, LightType::SPOT) {}

// ---------------------------------------------------------------------------------------------------------------------
