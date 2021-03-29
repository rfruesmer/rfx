#pragma once

#include "rfx/scene/PointLight.h"


namespace rfx {

class SpotLight : public PointLight
{
public:
    explicit SpotLight(const std::string& id) : PointLight(LightType::SPOT, id) {}

    void setDirection(const glm::vec3& direction);
    [[nodiscard]] const glm::vec3& getDirection() const;

    void setInnerConeAngle(float angle);
    [[nodiscard]] float getInnerConeAngle() const;

    void setOuterConeAngle(float angle);
    [[nodiscard]] float getOuterConeAngle() const;

private:
    glm::vec3 direction_ { 0.0f, -1.0f, 0.0f };

    float innerConeAngle = 0.0f;
    float outerConeAngle = glm::radians(90.0f);
};

using SpotLightPtr = std::shared_ptr<SpotLight>;

} // namespace rfx