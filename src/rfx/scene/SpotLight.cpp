#include "rfx/pch.h"
#include "rfx/scene/SpotLight.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

void SpotLight::setDirection(const vec3& direction)
{
    direction_ = direction;
}

// ---------------------------------------------------------------------------------------------------------------------

const vec3& SpotLight::getDirection() const
{
    return direction_;
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLight::setInnerConeAngle(float angle)
{
    innerConeAngle = angle;
}

// ---------------------------------------------------------------------------------------------------------------------

float SpotLight::getInnerConeAngle() const
{
    return innerConeAngle;
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLight::setOuterConeAngle(float angle)
{
    outerConeAngle = angle;
}

// ---------------------------------------------------------------------------------------------------------------------

float SpotLight::getOuterConeAngle() const
{
    return outerConeAngle;
}

// ---------------------------------------------------------------------------------------------------------------------

