#pragma once

#include "rfx/graphics/effect/Light.h"

namespace rfx
{

class PointLight : public Light
{
public:
    explicit PointLight(const std::string& id);

    void setPosition(float x, float y, float z);
    void setPosition(const glm::vec3& position);
    [[nodiscard]] const glm::vec3& getPosition() const;

    void setAttenuation(float constant, float linear, float quadratic);

protected:
    explicit PointLight(const std::string& id, LightType type);
};

}
