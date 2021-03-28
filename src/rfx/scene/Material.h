#pragma once

#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/graphics/VertexFormat.h"
#include "rfx/graphics/Texture2D.h"
#include "rfx/graphics/Buffer.h"


namespace rfx {

class Material
{
public:
    Material(
        std::string id,
        const VertexFormat& vertexFormat,
        std::string shaderId);

    [[nodiscard]] const VertexFormat& getVertexFormat() const;
    [[nodiscard]] const std::string& getShaderId() const;

    void setBaseColorFactor(const glm::vec4& baseColorFactor);
    [[nodiscard]] const glm::vec4& getBaseColorFactor() const;

    void setBaseColorTexture(std::shared_ptr<Texture2D> texture, uint32_t texCoordSet = 0);
    [[nodiscard]] const std::shared_ptr<Texture2D>& getBaseColorTexture() const;
    [[nodiscard]] int getBaseColorTexCoordSet() const;

    void setMetallicRoughnessTexture(std::shared_ptr<Texture2D> texture, uint32_t texCoordSet = 0);
    [[nodiscard]] const std::shared_ptr<Texture2D>& getMetallicRoughnessTexture() const;
    [[nodiscard]] int getMetallicRoughnessTexCoordSet() const;

    void setMetallicFactor(float factor);
    [[nodiscard]] float getMetallicFactor() const;

    void setRoughnessFactor(float factor);
    [[nodiscard]] float getRoughnessFactor() const;

    void setNormalTexture(std::shared_ptr<Texture2D> texture, uint32_t texCoordSet = 0);
    [[nodiscard]] const std::shared_ptr<Texture2D>& getNormalTexture() const;
    [[nodiscard]] int getNormalTexCoordSet() const;

    void setOcclusionTexture(
        std::shared_ptr<Texture2D> texture,
        uint32_t texCoordSet = 0,
        float strength = 1.0f);
    [[nodiscard]] const std::shared_ptr<Texture2D>& getOcclusionTexture() const;
    [[nodiscard]] int getOcclusionTexCoordSet() const;
    void setOcclusionStrength(float occlusionStrength);
    [[nodiscard]] float getOcclusionStrength() const;

    void setEmissiveTexture(std::shared_ptr<Texture2D> emissiveTexture, uint32_t texCoordSet = 0);
    [[nodiscard]] const std::shared_ptr<Texture2D>& getEmissiveTexture() const;
    [[nodiscard]] int getEmissiveTexCoordSet() const;

    void setEmissiveFactor(const glm::vec3& factor);
    [[nodiscard]] const glm::vec3& getEmissiveFactor();

    void setSpecularFactor(const glm::vec3& specularFactor);
    [[nodiscard]] const glm::vec3& getSpecularFactor() const;

    void setShininess(float shininess);
    [[nodiscard]] float getShininess() const;

    void setUniformBuffer(const std::shared_ptr<Buffer>& uniformBuffer);
    [[nodiscard]] const std::shared_ptr<Buffer>& getUniformBuffer() const;

    void loadUniformBufferData(const std::vector<std::byte>& data);

    void setDescriptorSet(VkDescriptorSet descriptorSet);
    [[nodiscard]] VkDescriptorSet getDescriptorSet() const;

private:
    std::string id_;

    const VertexFormat vertexFormat_;
    const std::string shaderId_;

    glm::vec4 baseColorFactor_ { 1.0f };
    std::shared_ptr<Texture2D> baseColorTexture_;
    int baseColorTexCoordSet_ = -1;

    std::shared_ptr<Texture2D> metallicRoughnessTexture_;
    int metallicRoughnessTexCoordSet_ = -1;
    float metallicFactor_ = 1.0f;
    float roughnessFactor_ = 0.0f;

    std::shared_ptr<Texture2D> normalTexture_;
    int normalTexCoordSet_ = -1;

    std::shared_ptr<Texture2D> occlusionTexture_;
    int occlusionTexCoordSet_ = -1;
    float occlusionStrength_ = 1.0f;

    glm::vec3 emissiveFactor_ { 0.0f };
    std::shared_ptr<Texture2D> emissiveTexture_;
    int emissiveTexCoordSet_ = -1;

    glm::vec3 specularFactor_ { 0.0f };
    float shininess_ = 0.0f; // 0-128

    VkDescriptorSet descriptorSet_ = VK_NULL_HANDLE;
    std::shared_ptr<Buffer> uniformBuffer_; // TODO: consider refactoring to push constants or refactor to sub-buffer allocation
};

using MaterialPtr = std::shared_ptr<Material>;

} // namespace rfx
