#pragma once

namespace rfx
{

class Material
{
public:
    struct Data
    {
        glm::vec4 ambient = { 0.0F, 0.0F, 0.0F, 1.0F };
        glm::vec4 diffuse = { 0.0F, 0.0F, 0.0F, 1.0F };
        glm::vec4 specular = { 0.0F, 0.0F, 0.0F, 1.0F };
        glm::vec4 emissive = { 0.0F, 0.0F, 0.0F, 1.0F };
        float shininess = 0.0F;
    };

    Material() = default;
    explicit Material(const Data& data);

    void setAmbient(const glm::vec4& color);
    [[nodiscard]] const glm::vec4& getAmbient() const;

    void setDiffuse(const glm::vec4& color);
    [[nodiscard]] const glm::vec4& getDiffuse() const;

    void setSpecular(const glm::vec4& color);
    [[nodiscard]] const glm::vec4& getSpecular() const;

    void setEmissive(const glm::vec4& color);
    [[nodiscard]] const glm::vec4& getEmissive() const;

    void setShininess(float value);
    [[nodiscard]] float getShininess() const;

    const Data& getData() const;

private:
    Data data;
};

}
