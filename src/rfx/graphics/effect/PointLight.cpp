#include "rfx/pch.h"
#include "rfx/graphics/effect/PointLight.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

PointLight::PointLight(const string& id)
    : Light(id, LightType::POINT) {}

// ---------------------------------------------------------------------------------------------------------------------
