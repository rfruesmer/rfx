#pragma once

namespace rfx
{

class Transform
{
public:
    bool isIdentity() const;

    void setTranslation(const glm::vec3& translation);
    const glm::vec3& getTranslation() const;

    void setScale(const glm::vec3& scale);
    const glm::vec3& getScale() const;

    void setRotation(const glm::vec3& rotation);
    const glm::vec3& getRotation() const;

    void update();

private:
    glm::vec3 translation = glm::vec4(0.0F);
    glm::vec3 scale = glm::vec4(1.0F);
    glm::vec3 rotation = glm::vec4(0.0F);
    glm::mat4 matrix = glm::mat4(1.0F);
    bool dirty = false;
    bool identity = true;
};

}

