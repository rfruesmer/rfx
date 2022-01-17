#pragma once


namespace rfx {

class Light
{
public:
    enum LightType {
        POINT = 0,
        SPOT,
        DIRECTIONAL
    };

    explicit Light(LightType type, std::string id);
    virtual ~Light() = default;

    [[nodiscard]] LightType getType() const;

    void setEnabled(bool enabled);
    [[nodiscard]] bool isEnabled() const;

    void setColor(const glm::vec3& color);
    [[nodiscard]] const glm::vec3& getColor() const;

private:
    std::string id_;
    LightType type_;
    bool enabled_ = true;
    glm::vec3 color_ { 0.0f };
};

using LightPtr = std::shared_ptr<Light>;

} // namespace rfx
