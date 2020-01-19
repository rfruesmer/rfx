#include "rfx/pch.h"
#include "rfx/scene/Transform.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

bool Transform::isIdentity() const
{
    return identity;
}

// ---------------------------------------------------------------------------------------------------------------------

void Transform::setTranslation(const vec3& translation)
{
    this->translation = vec4(translation, 1.0F);

    identity =  false;
    dirty = true;
}

// ---------------------------------------------------------------------------------------------------------------------

const vec3& Transform::getTranslation() const
{
    return translation;
}

// ---------------------------------------------------------------------------------------------------------------------

void Transform::setScale(const vec3& scale)
{
    this->scale = vec4(scale, 1.0F);

    identity =  false;
    dirty = true;
}

// ---------------------------------------------------------------------------------------------------------------------

const vec3& Transform::getScale() const
{
    return scale;
}

// ---------------------------------------------------------------------------------------------------------------------

void Transform::setRotation(const vec3& rotation)
{
    this->rotation = vec4(rotation, 1.0F);

    identity =  false;
    dirty = true;
}

// ---------------------------------------------------------------------------------------------------------------------

const vec3& Transform::getRotation() const
{
    return rotation;
}

// ---------------------------------------------------------------------------------------------------------------------

const mat4& Transform::getMatrix() const
{
    return matrix;
}

// ---------------------------------------------------------------------------------------------------------------------

void Transform::update()
{
    matrix = glm::translate(mat4(1.0F), translation);
    matrix = glm::rotate(matrix, radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
    matrix = glm::scale(matrix, scale);

    dirty = false;
}

// ---------------------------------------------------------------------------------------------------------------------
