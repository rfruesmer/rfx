#include "rfx/pch.h"
#include "rfx/scene/Camera.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

void Camera::setPosition(float x, float y, float z)
{
    position.x = x;
    position.y = -1.0F * y;
    position.z = z;
}

// ---------------------------------------------------------------------------------------------------------------------

void Camera::setPosition(const vec3& position)
{
    this->position = position;
    this->position.y *= -1.0F;
}

// ---------------------------------------------------------------------------------------------------------------------

const vec3& Camera::getPosition() const
{
    return position;
}

// ---------------------------------------------------------------------------------------------------------------------

void Camera::setLookAt(float x, float y, float z)
{
    lookAt.x = x;
    lookAt.y = y;
    lookAt.z = z;
}

// ---------------------------------------------------------------------------------------------------------------------

void Camera::setLookAt(const vec3& lookAt)
{
    this->lookAt = lookAt;
}

// ---------------------------------------------------------------------------------------------------------------------

const vec3& Camera::getLookAt() const
{
    return lookAt;
}

// ---------------------------------------------------------------------------------------------------------------------

void Camera::setUp(float x, float y, float z)
{
    up.x = x;
    up.y = y;
    up.z = z;
}

// ---------------------------------------------------------------------------------------------------------------------

void Camera::setUp(const vec3& up)
{
    this->up = up;
}

// ---------------------------------------------------------------------------------------------------------------------

void Camera::setProjection(float fovDeg, float aspect, float nearZ, float farZ)
{
    projectionMatrix = perspective(radians(fovDeg), aspect, nearZ, farZ);
}

// ---------------------------------------------------------------------------------------------------------------------

void Camera::update()
{
    viewMatrix = glm::lookAt(position, lookAt, up);
    viewProjMatrix = projectionMatrix * viewMatrix;
}

// ---------------------------------------------------------------------------------------------------------------------

const mat4& Camera::getViewMatrix() const
{
    return viewMatrix;
}

// ---------------------------------------------------------------------------------------------------------------------

const mat4& Camera::getViewProjMatrix() const
{
    return viewProjMatrix;
}

// ---------------------------------------------------------------------------------------------------------------------
