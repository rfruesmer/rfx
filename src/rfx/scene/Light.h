#pragma once


namespace rfx {

class Light
{
public:
    enum LightType {
        POINT = 0,
        SPOT
    };

    explicit Light(LightType type, std::string id);
    virtual ~Light() = default;

    [[nodiscard]] LightType getType() const;

    void setColor(const glm::vec3& color);
    [[nodiscard]] const glm::vec3& getColor() const;

private:
    std::string id_;
    LightType type_;
    glm::vec3 color_ { 0.0f };
};

} // namespace rfx
