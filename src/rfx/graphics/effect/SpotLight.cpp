#include "rfx/pch.h"
#include "rfx/graphics/effect/SpotLight.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

SpotLight::SpotLight(const string& id)
    : PointLight(id, LightType::SPOT) {}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLight::setDirection(float x, float y, float z)
{
    setDirection(vec3(x, y, z));
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLight::setDirection(const vec3& direction)
{
    data.direction = normalize(direction);
//    data.direction.y *= -1.0F;
}

// ---------------------------------------------------------------------------------------------------------------------

const vec3& SpotLight::getDirection() const
{
    return data.direction;
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLight::setExponent(float value)
{
    data.spotExponent = value;
}

// ---------------------------------------------------------------------------------------------------------------------

float SpotLight::getExponent() const
{
    return data.spotExponent;
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLight::setCutoff(float value)
{
    data.spotCutoff = value;
}

// ---------------------------------------------------------------------------------------------------------------------

float SpotLight::getCutoff() const
{
    return data.spotCutoff;
}

// ---------------------------------------------------------------------------------------------------------------------
