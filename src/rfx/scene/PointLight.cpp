#include "rfx/pch.h"
#include "rfx/scene/PointLight.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

void PointLight::setPosition(const vec3& position)
{
    PointLight::position = position;
}

// ---------------------------------------------------------------------------------------------------------------------

const vec3& PointLight::getPosition() const
{
    return position;
}

// ---------------------------------------------------------------------------------------------------------------------
