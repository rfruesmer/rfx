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

void SpotLight::setExponent(float exponent)
{
    exponent_ = exponent;
}

// ---------------------------------------------------------------------------------------------------------------------

float SpotLight::getExponent() const
{
    return exponent_;
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLight::setCutoff(float cutoff)
{
    cutoff_ = cutoff;
}

// ---------------------------------------------------------------------------------------------------------------------

float SpotLight::getCutoff() const
{
    return cutoff_;
}

// ---------------------------------------------------------------------------------------------------------------------

