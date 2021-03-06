#pragma once

#include "rfx/scene/Light.h"

namespace rfx {

class PointLight : public Light
{
public:
    PointLight() : Light(LightType::POINT) {}

    void setPosition(const glm::vec3& position);
    [[nodiscard]] const glm::vec3& getPosition() const;

protected:
    explicit PointLight(LightType type) : Light(type) {}

private:
    glm::vec3 position { 0.0f };
};

} // namespace rfx
