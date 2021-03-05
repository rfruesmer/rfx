#pragma once


namespace rfx {

class Light
{
public:
    void setColor(const glm::vec3& color);
    [[nodiscard]] const glm::vec3& getColor() const;

private:
    glm::vec3 color_ {0.0f };
};

} // namespace rfx
