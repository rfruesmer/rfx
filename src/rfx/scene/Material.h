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

    void setMetallicRoughnessTexture(std::shared_ptr<Texture2D> texture);
    [[nodiscard]] const std::shared_ptr<Texture2D>& getMetallicRoughnessTexture() const;

    void setMetallicFactor(float factor);
    [[nodiscard]] float getMetallicFactor() const;

    void setRoughnessFactor(float factor);
    [[nodiscard]] float getRoughnessFactor() const;

    void setNormalTexture(const std::shared_ptr<Texture2D>& texture);
    [[nodiscard]] const std::shared_ptr<Texture2D>& getNormalTexture() const;

    void setSpecularFactor(const glm::vec3& specularFactor);
    [[nodiscard]] const glm::vec3& getSpecularFactor() const;

    void setShininess(float shininess);
    [[nodiscard]] float getShininess() const;

private:
    glm::vec4 baseColorFactor_ { 1.0f };
    std::shared_ptr<Texture2D> baseColorTexture_;
    std::shared_ptr<Texture2D> metallicRoughnessTexture_;
    float metallicFactor_ = 1.0f;
    float roughnessFactor_ = 0.0f;
    std::shared_ptr<Texture2D> normalTexture_;

    glm::vec3 specularFactor_ { 0.0f };
    float shininess_ = 0.0f; // 0-128
};

} // namespace rfx
