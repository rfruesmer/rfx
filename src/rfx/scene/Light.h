#pragma once


namespace rfx {

class Light
{
public:
    void setAmbient(const glm::vec3& ambient);
    [[nodiscard]] const glm::vec3& getAmbient() const;

    void setDiffuse(const glm::vec3& diffuse);
    [[nodiscard]] const glm::vec3& getDiffuse() const;

    void setSpecular(const glm::vec3& specular);
    [[nodiscard]] const glm::vec3& getSpecular() const;

private:
    glm::vec3 ambient { 0.0f };
    glm::vec3 diffuse { 0.0f };
    glm::vec3 specular { 0.0f };
};

} // namespace rfx
