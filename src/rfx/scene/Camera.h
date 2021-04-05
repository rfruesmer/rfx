#pragma once


namespace rfx {

class Camera
{
public:
    [[nodiscard]] virtual const glm::vec3& getPosition() const = 0;
    [[nodiscard]] virtual const glm::mat4& getViewMatrix() const = 0;

    const glm::mat4& getProjectionMatrix() const;

    void setProjectionMatrix(const glm::mat4& projectionMatrix);

private:
    glm::mat4 projectionMatrix;
};

using CameraPtr = std::shared_ptr<Camera>;

} // namespace rfx
