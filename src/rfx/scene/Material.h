#pragma once

#include "rfx/graphics/Texture2D.h"

namespace rfx {

class Material
{
public:
    explicit Material(std::string id);

    void setBaseColorFactor(const glm::vec4& baseColorFactor);
    [[nodiscard]] const glm::vec4& getBaseColorFactor() const;

    void setBaseColorTexture(std::shared_ptr<Texture2D> texture, uint32_t texCoordSet = 0);
    [[nodiscard]] const std::shared_ptr<Texture2D>& getBaseColorTexture() const;
    [[nodiscard]] uint32_t getBaseColorTexCoordSet() const;

    void setMetallicRoughnessTexture(std::shared_ptr<Texture2D> texture, uint32_t texCoordSet = 0);
    [[nodiscard]] const std::shared_ptr<Texture2D>& getMetallicRoughnessTexture() const;
    [[nodiscard]] uint32_t getMetallicRoughnessTexCoordSet() const;

    void setMetallicFactor(float factor);
    [[nodiscard]] float getMetallicFactor() const;

    void setRoughnessFactor(float factor);
    [[nodiscard]] float getRoughnessFactor() const;

    void setNormalTexture(std::shared_ptr<Texture2D> texture, uint32_t texCoordSet = 0);
    [[nodiscard]] const std::shared_ptr<Texture2D>& getNormalTexture() const;
    [[nodiscard]] uint32_t getNormalTexCoordSet() const;

    void setEmissiveTexture(std::shared_ptr<Texture2D> emissiveTexture, uint32_t texCoordSet = 0);
    [[nodiscard]] const std::shared_ptr<Texture2D>& getEmissiveTexture() const;
    [[nodiscard]] uint32_t getEmissiveTexCoordSet() const;

    void setEmissiveFactor(const glm::vec3& factor);
    [[nodiscard]] const glm::vec3& getEmissiveFactor();

    void setSpecularFactor(const glm::vec3& specularFactor);
    [[nodiscard]] const glm::vec3& getSpecularFactor() const;

    void setShininess(float shininess);
    [[nodiscard]] float getShininess() const;

private:
    std::string id_;

    glm::vec4 baseColorFactor_ { 1.0f };
    std::shared_ptr<Texture2D> baseColorTexture_;
    uint32_t baseColorTexCoordSet_ = 0;

    std::shared_ptr<Texture2D> metallicRoughnessTexture_;
    uint32_t metallicRoughnessTexCoordSet_ = 0;
    float metallicFactor_ = 1.0f;
    float roughnessFactor_ = 0.0f;

    std::shared_ptr<Texture2D> normalTexture_;
    uint32_t normalTexCoordSet_ = 0;

    glm::vec3 emissiveFactor_ { 0.0f };
    std::shared_ptr<Texture2D> emissiveTexture_;
    uint32_t emissiveTexCoordSet_ = 0;

    glm::vec3 specularFactor_ { 0.0f };
    float shininess_ = 0.0f; // 0-128
};

} // namespace rfx
