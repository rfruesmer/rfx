#pragma once

#include "rfx/scene/Light.h"

namespace rfx {

class PointLight : public Light
{
public:
    void setPosition(const glm::vec3& position);
    [[nodiscard]] const glm::vec3& getPosition() const;

private:
    glm::vec3 position { 0.0f };
};

} // namespace rfx
