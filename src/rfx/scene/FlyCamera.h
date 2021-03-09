#pragma once


namespace rfx {

class FlyCamera
{
    static const inline glm::vec3 UNIT_X { 1.0f, 0.0f, 0.0f };
    static const inline glm::vec3 UNIT_Y { 0.0f, 1.0f, 0.0f };
    static const inline glm::vec3 UNIT_Z { 0.0f, 0.0f, 1.0f };

public:
    void setPosition(const glm::vec3& position);
    void setVelocity(const glm::vec3& velocity);

    void addYaw(float yaw);
    void addPitch(float pitch);

    void update(float deltaTime);

    [[nodiscard]] const glm::mat4& getViewMatrix() const;
    [[nodiscard]] const glm::vec3& getPosition() const;

private:
    float pitch_ = 0.0f;
    float yaw_ = 0.0f;
    glm::vec3 position_ {};
    glm::vec3 velocity_ {};
    glm::mat4 viewMatrix_ {};
};

} // namespace rfx