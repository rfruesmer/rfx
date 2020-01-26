#include "rfx/pch.h"
#include "rfx/graphics/effect/DirectionalLight.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

DirectionalLight::DirectionalLight(const string& id)
    : Light(id, LightType::DIRECTIONAL) {}

// ---------------------------------------------------------------------------------------------------------------------

void DirectionalLight::setDirection(float x, float y, float z)
{
    setDirection(vec3(x, y ,z));
}

// ---------------------------------------------------------------------------------------------------------------------

void DirectionalLight::setDirection(const vec3& direction)
{
    data.position = direction;
    data.position.y *= -1.0F;
}

// ---------------------------------------------------------------------------------------------------------------------

const vec3& DirectionalLight::getDirection() const
{
    return data.position;
}

// ---------------------------------------------------------------------------------------------------------------------
