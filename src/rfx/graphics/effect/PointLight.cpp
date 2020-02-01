#include "rfx/pch.h"
#include "rfx/graphics/effect/PointLight.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

PointLight::PointLight(const string& id)
    : Light(id, LightType::POINT) {}

// ---------------------------------------------------------------------------------------------------------------------

void PointLight::setPosition(float x, float y, float z)
{
    setPosition(vec3(x, y ,z));
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLight::setPosition(const vec3& position)
{
    data.position = position;
//    data.position.y *= -1.0F;
}

// ---------------------------------------------------------------------------------------------------------------------

const vec3& PointLight::getPosition() const
{
    return data.position;
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLight::setAttenuation(float constant, float linear, float quadratic)
{
    data.attenuation.x = constant;
    data.attenuation.y = linear;
    data.attenuation.z = quadratic;
}

// ---------------------------------------------------------------------------------------------------------------------
