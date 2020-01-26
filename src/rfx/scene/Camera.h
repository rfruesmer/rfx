#pragma once

namespace rfx
{
    
class Camera
{
public:
    void setPosition(float x, float y, float z);
    void setPosition(const glm::vec3& position);
    const glm::vec3& getPosition() const;

    void setLookAt(float x, float y, float z);
    void setLookAt(const glm::vec3& lookAt);
    const glm::vec3& getLookAt() const;

    void setUp(float x, float y, float z);
    void setUp(const glm::vec3& up);

    void setProjection(float fovDeg, float aspect, float nearZ, float farZ);

    void update();

    const glm::mat4& getViewProjMatrix() const;

private:
    glm::vec3 position = glm::vec3(0.0F);
    glm::vec3 lookAt = glm::vec3(0.0F);
    glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F);

    glm::mat4 viewMatrix = glm::mat4(1.0F);
    glm::mat4 projectionMatrix = glm::mat4(1.0F);
    glm::mat4 viewProjMatrix = glm::mat4(1.0F);
};

} // namespace rfx
