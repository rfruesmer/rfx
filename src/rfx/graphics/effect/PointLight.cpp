#include "rfx/pch.h"
#include "rfx/graphics/effect/PointLight.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

PointLight::PointLight(const string& id)
    : Light(id, LightType::POINT) {}

// ---------------------------------------------------------------------------------------------------------------------

PointLight::PointLight(const std::string& id, LightType type)
    : Light(id, type) {}

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
    data.attenuationFactors.x = constant;
    data.attenuationFactors.y = linear;
    data.attenuationFactors.z = quadratic;
}

// ---------------------------------------------------------------------------------------------------------------------
