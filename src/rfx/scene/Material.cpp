#include "rfx/pch.h"
#include "rfx/scene/Material.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

void Material::setBaseColorFactor(const glm::vec4& baseColorFactor)
{
    this->baseColorFactor = baseColorFactor;
}

// ---------------------------------------------------------------------------------------------------------------------

const glm::vec4& Material::getBaseColorFactor() const
{
    return baseColorFactor;
}

// ---------------------------------------------------------------------------------------------------------------------

void Material::setBaseColorTexture(std::shared_ptr<Texture2D> texture)
{
    baseColorTexture = move(texture);
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Texture2D>& Material::getBaseColorTexture() const
{
    return baseColorTexture;
}

// ---------------------------------------------------------------------------------------------------------------------

void Material::setShininess(float shininess)
{
    Material::shininess = shininess;
}

// ---------------------------------------------------------------------------------------------------------------------

float Material::getShininess() const
{
    return shininess;
}

// ---------------------------------------------------------------------------------------------------------------------

void Material::setNormalTexture(const shared_ptr<Texture2D>& texture)
{
    normalTexture = texture;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Texture2D>& Material::getNormalTexture() const
{
    return normalTexture;
}

// ---------------------------------------------------------------------------------------------------------------------

