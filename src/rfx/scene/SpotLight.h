#pragma once

#include "rfx/scene/PointLight.h"


namespace rfx {

class SpotLight : public PointLight
{
public:
    SpotLight() : PointLight(LightType::SPOT) {}

    void setDirection(const glm::vec3& direction);
    [[nodiscard]] const glm::vec3& getDirection() const;

    void setExponent(float exponent);
    [[nodiscard]] float getExponent() const;

    void setCutoff(float cutoff);
    [[nodiscard]] float getCutoff() const;

private:
    glm::vec3 direction_;
    float exponent_ = 0.0f; // 0-128
    float cutoff_ = 0.0f; // 0-90
};

} // namespace rfx