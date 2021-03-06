#include "rfx/pch.h"
#include "rfx/scene/Light.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

Light::Light(LightType type, string id)
    : id_(move(id)),
      type_(type) {}

// ---------------------------------------------------------------------------------------------------------------------

Light::LightType Light::getType() const
{
    return type_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Light::setColor(const vec3& color)
{
    color_ = color;
}

// ---------------------------------------------------------------------------------------------------------------------

const vec3& Light::getColor() const
{
    return color_;
}

// ---------------------------------------------------------------------------------------------------------------------

