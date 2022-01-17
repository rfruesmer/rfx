#pragma once

#include "rfx/scene/Light.h"

namespace rfx {

class DirectionalLight : public Light
{
public:
    explicit DirectionalLight(const std::string& id) : Light(LightType::DIRECTIONAL, id) {}

    void setDirection(const glm::vec3& direction);
    [[nodiscard]] const glm::vec3& getDirection() const;

protected:
    DirectionalLight(LightType type, const std::string& id) : Light(type, id) {}

private:
    glm::vec3 direction_ { 0.0f };
};

using DirectionalLightPtr = std::shared_ptr<DirectionalLight>;

} // namespace rfx
