#pragma once

#include "rfx/graphics/Texture2D.h"

namespace rfx {

class Material
{
public:
    Material() = default;

    void setBaseColorFactor(const glm::vec4& baseColorFactor);
    [[nodiscard]] const glm::vec4& getBaseColorFactor() const;

    void setBaseColorTexture(std::shared_ptr<Texture2D> texture);
    [[nodiscard]] const std::shared_ptr<Texture2D>& getBaseColorTexture() const;

    float getShininess() const;

    void setShininess(float shininess);

private:
    glm::vec4 baseColorFactor = glm::vec4(1.0f);
    std::shared_ptr<Texture2D> baseColorTexture;

    float shininess = 0.0f; // 0-128
};

} // namespace rfx
