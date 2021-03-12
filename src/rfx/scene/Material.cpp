#include "rfx/pch.h"
#include "rfx/scene/Material.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

void Material::setBaseColorFactor(const vec4& baseColorFactor)
{
    baseColorFactor_ = baseColorFactor;
}

// ---------------------------------------------------------------------------------------------------------------------

const vec4& Material::getBaseColorFactor() const
{
    return baseColorFactor_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Material::setBaseColorTexture(shared_ptr<Texture2D> texture)
{
    baseColorTexture_ = move(texture);
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Texture2D>& Material::getBaseColorTexture() const
{
    return baseColorTexture_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Material::setMetallicRoughnessTexture(shared_ptr<Texture2D> texture)
{
    metallicRoughnessTexture_ = move(texture);
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Texture2D>& Material::getMetallicRoughnessTexture() const
{
    return metallicRoughnessTexture_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Material::setMetallicFactor(float factor)
{
    metallicFactor_ = factor;
}

// ---------------------------------------------------------------------------------------------------------------------

float Material::getMetallicFactor() const
{
    return metallicFactor_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Material::setRoughnessFactor(float factor)
{
    roughnessFactor_ = factor;
}

// ---------------------------------------------------------------------------------------------------------------------

float Material::getRoughnessFactor() const
{
    return roughnessFactor_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Material::setShininess(float shininess)
{
    shininess_ = shininess;
}

// ---------------------------------------------------------------------------------------------------------------------

float Material::getShininess() const
{
    return shininess_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Material::setNormalTexture(const shared_ptr<Texture2D>& texture)
{
    normalTexture_ = texture;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Texture2D>& Material::getNormalTexture() const
{
    return normalTexture_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Material::setSpecularFactor(const vec3& specularFactor)
{
    specularFactor_ = specularFactor;
}

// ---------------------------------------------------------------------------------------------------------------------

const vec3& Material::getSpecularFactor() const
{
    return specularFactor_;
}

// ---------------------------------------------------------------------------------------------------------------------

