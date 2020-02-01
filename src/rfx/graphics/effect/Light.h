#pragma once

namespace rfx
{

enum class LightType : uint32_t
{
    DIRECTIONAL = 0,
    POINT,
    SPOT
};

class Light
{
public:
    struct Data
    {
        LightType type;
        glm::vec3 position = { 0.0F, 0.0F, 0.0F };
        glm::vec4 ambient = { 0.0F, 0.0F, 0.0F, 1.0F };
        glm::vec4 diffuse = { 0.0F, 0.0F, 0.0F, 1.0F };
        glm::vec4 specular = { 0.0F, 0.0F, 0.0F, 1.0F };
        glm::vec3 attenuation = { 1.0F, 0.0F, 0.0F };
    };

    Light(const std::string& id, LightType type);
    virtual ~Light() = default;

    const std::string& getId() const;
    [[nodiscard]] LightType getType() const;

    void setAmbient(float red, float green, float blue);
    void setAmbient(const glm::vec4& color);
    [[nodiscard]] const glm::vec4& getAmbient() const;

    void setDiffuse(float red, float green, float blue);
    void setDiffuse(const glm::vec4& color);
    [[nodiscard]] const glm::vec4& getDiffuse() const;

    void setSpecular(float red, float green, float blue);
    void setSpecular(const glm::vec4& color);
    [[nodiscard]] const glm::vec4& getSpecular() const;

    const Data& getData() const;

protected:
    Data data;

private:
    std::string id;
};

} // namespace rfx

