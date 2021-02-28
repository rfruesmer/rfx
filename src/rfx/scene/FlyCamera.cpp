#include "rfx/pch.h"
#include "rfx/scene/FlyCamera.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

void FlyCamera::setPosition(const vec3& position)
{
    position_ = position;
}

// ---------------------------------------------------------------------------------------------------------------------

void FlyCamera::setVelocity(const vec3& velocity)
{
    velocity_ = velocity;
}

// ---------------------------------------------------------------------------------------------------------------------

void FlyCamera::addYaw(float yaw)
{
    yaw_ += yaw;
}

// ---------------------------------------------------------------------------------------------------------------------

void FlyCamera::addPitch(float pitch)
{
    pitch_ += pitch;
    pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void FlyCamera::update(float deltaTime)
{
    const quat pitch = angleAxis(radians(pitch_), UNIT_X);
    const quat yaw = angleAxis(radians(yaw_), UNIT_Y);
    const quat orientation = normalize(pitch * yaw);
    const quat inverseOrientation = inverse(orientation);
    const mat4 rotation = mat4_cast(orientation);

    vec3 forward = rotate(inverseOrientation, UNIT_Z);
    position_ += forward * velocity_.z * deltaTime;

    vec3 right = rotate(inverseOrientation, UNIT_X);
    position_ += right * velocity_.x * deltaTime;

    vec3 up = rotate(inverseOrientation, UNIT_Y);
    position_ += up * velocity_.y * deltaTime;

    mat4 translation { 1.0f };
    translation = translate(translation, -position_);

    viewMatrix_ = rotation * translation;
}

// ---------------------------------------------------------------------------------------------------------------------

const mat4& FlyCamera::getViewMatrix() const
{
    return viewMatrix_;
}

// ---------------------------------------------------------------------------------------------------------------------
