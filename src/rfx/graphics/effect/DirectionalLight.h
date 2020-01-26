#pragma once

#include "rfx/graphics/effect/Light.h"


namespace rfx
{

class DirectionalLight : public Light
{
public:
    explicit DirectionalLight(const std::string& id);

    void setDirection(float x, float y, float z);
    void setDirection(const glm::vec3& direction);
    [[nodiscard]] const glm::vec3& getDirection() const;
};

} // namespace rfx

