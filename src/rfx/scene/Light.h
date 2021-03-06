#pragma once


namespace rfx {

class Light
{
public:
    enum LightType {
        POINT = 0,
        SPOT
    };

    explicit Light(LightType type);

    LightType getType() const;

    void setColor(const glm::vec3& color);
    [[nodiscard]] const glm::vec3& getColor() const;

private:
    LightType type_;
    glm::vec3 color_ {0.0f };
};

} // namespace rfx
