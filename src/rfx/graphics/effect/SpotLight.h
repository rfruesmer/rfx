#pragma once

#include "rfx/graphics/effect/PointLight.h"

namespace rfx
{

class SpotLight : public PointLight
{
public:
    explicit SpotLight(const std::string& id);

    void setDirection(float x, float y, float z);
    void setDirection(const glm::vec3& direction);
    [[nodiscard]] const glm::vec3& getDirection() const;

    void setExponent(float value);
    float getExponent() const;

    void setCutoff(float value);
    float getCutoff() const;
};

}
