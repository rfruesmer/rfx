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

    void setNormalTexture(const std::shared_ptr<Texture2D>& texture);
    [[nodiscard]] const std::shared_ptr<Texture2D>& getNormalTexture() const;

    void setSpecularFactor(const glm::vec3& specularFactor);
    [[nodiscard]] const glm::vec3& getSpecularFactor() const;

    void setShininess(float shininess);
    [[nodiscard]] float getShininess() const;

private:
    glm::vec4 baseColorFactor_ { 1.0f };
    std::shared_ptr<Texture2D> baseColorTexture_;
    std::shared_ptr<Texture2D> normalTexture_;

    glm::vec3 specularFactor_ { 0.0f };
    float shininess_ = 0.0f; // 0-128
};

} // namespace rfx
