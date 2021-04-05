#include "rfx/pch.h"
#include "rfx/scene/Camera.h"


using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

void Camera::setProjectionMatrix(const mat4& projectionMatrix)
{
    this->projectionMatrix = projectionMatrix;
}

// ---------------------------------------------------------------------------------------------------------------------

const glm::mat4& Camera::getProjectionMatrix() const
{
    return projectionMatrix;
}

// ---------------------------------------------------------------------------------------------------------------------
