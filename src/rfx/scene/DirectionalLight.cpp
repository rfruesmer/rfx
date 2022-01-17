#include "rfx/pch.h"
#include "DirectionalLight.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

void DirectionalLight::setDirection(const vec3& direction)
{
    direction_ = direction;
}

// ---------------------------------------------------------------------------------------------------------------------

const vec3& DirectionalLight::getDirection() const
{
    return direction_;
}

// ---------------------------------------------------------------------------------------------------------------------

