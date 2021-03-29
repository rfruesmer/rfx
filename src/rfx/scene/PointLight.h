#pragma once

#include "rfx/scene/Light.h"

namespace rfx {

class PointLight : public Light
{
public:
    explicit PointLight(const std::string& id) : Light(LightType::POINT, id) {}

    void setPosition(const glm::vec3& position);
    [[nodiscard]] const glm::vec3& getPosition() const;

    void setRange(float range);
    [[nodiscard]] float getRange() const;

protected:
    explicit PointLight(LightType type, const std::string& id) : Light(type, id) {}

private:
    glm::vec3 position_ { 0.0f };
    float range_ = 0.0f;
};

using PointLightPtr = std::shared_ptr<PointLight>;

} // namespace rfx
