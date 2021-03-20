#include "rfx/pch.h"
#include "rfx/scene/Material.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

Material::Material(
    string id,
    const VertexFormat& vertexFormat,
    string vertexShaderId,
    string fragmentShaderId)
        : id_(move(id)),
          vertexFormat_(vertexFormat),
          vertexShaderId_(move(vertexShaderId)),
          fragmentShaderId_(move(fragmentShaderId)) {}

// ---------------------------------------------------------------------------------------------------------------------

const VertexFormat& Material::getVertexFormat() const
{
    return vertexFormat_;
}

// ---------------------------------------------------------------------------------------------------------------------

const string& Material::getVertexShaderId() const
{
    return vertexShaderId_;
}

// ---------------------------------------------------------------------------------------------------------------------

const string& Material::getFragmentShaderId() const
{
    return fragmentShaderId_;
}

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

void Material::setBaseColorTexture(
    shared_ptr<Texture2D> texture,
    uint32_t texCoordSet)
{
    baseColorTexture_ = move(texture);
    baseColorTexCoordSet_ = texCoordSet;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Texture2D>& Material::getBaseColorTexture() const
{
    return baseColorTexture_;
}

// ---------------------------------------------------------------------------------------------------------------------

int Material::getBaseColorTexCoordSet() const
{
    return baseColorTexCoordSet_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Material::setMetallicRoughnessTexture(
    shared_ptr<Texture2D> texture,
    uint32_t texCoordSet)
{
    metallicRoughnessTexture_ = move(texture);
    metallicRoughnessTexCoordSet_ = texCoordSet;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Texture2D>& Material::getMetallicRoughnessTexture() const
{
    return metallicRoughnessTexture_;
}

// ---------------------------------------------------------------------------------------------------------------------

int Material::getMetallicRoughnessTexCoordSet() const
{
    return metallicRoughnessTexCoordSet_;
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

void Material::setNormalTexture(
    shared_ptr<Texture2D> texture,
    uint32_t texCoordSet)
{
    normalTexture_ = move(texture);
    normalTexCoordSet_ = texCoordSet;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Texture2D>& Material::getNormalTexture() const
{
    return normalTexture_;
}

// ---------------------------------------------------------------------------------------------------------------------

int Material::getNormalTexCoordSet() const
{
    return normalTexCoordSet_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Material::setOcclusionTexture(
    shared_ptr<Texture2D> texture,
    uint32_t texCoordSet,
    float strength)
{
    occlusionTexture_ = move(texture);
    occlusionTexCoordSet_ = texCoordSet;
    occlusionStrength_ = strength;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::shared_ptr<Texture2D>& Material::getOcclusionTexture() const
{
    return occlusionTexture_;
}

// ---------------------------------------------------------------------------------------------------------------------

float Material::getOcclusionStrength() const
{
    return occlusionStrength_;
}

// ---------------------------------------------------------------------------------------------------------------------

int Material::getOcclusionTexCoordSet() const
{
    return occlusionTexCoordSet_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Material::setEmissiveTexture(
    shared_ptr<Texture2D> texture,
    uint32_t texCoordSet)
{
    emissiveTexture_ = move(texture);
    emissiveTexCoordSet_ = texCoordSet;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Texture2D>& Material::getEmissiveTexture() const
{
    return emissiveTexture_;
}

// ---------------------------------------------------------------------------------------------------------------------

int Material::getEmissiveTexCoordSet() const
{
    return emissiveTexCoordSet_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Material::setEmissiveFactor(const vec3& factor)
{
    emissiveFactor_ = factor;
}

// ---------------------------------------------------------------------------------------------------------------------

const vec3& Material::getEmissiveFactor()
{
    return emissiveFactor_;
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

