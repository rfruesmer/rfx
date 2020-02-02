#include "rfx/pch.h"
#include "rfx/graphics/effect/LightDefinitionDeserializer.h"
#include "rfx/graphics/effect/DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

using namespace rfx;
using namespace glm;
using namespace std;


static const unordered_map<string, LightType> lightTypeMap = {
    { "directional", LightType::DIRECTIONAL },
    { "point", LightType::POINT },
    { "spot", LightType::SPOT }
};

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Light> LightDefinitionDeserializer::deserialize(const Json::Value& jsonLight) const
{
    const string lightTypeString = jsonLight["type"].asString();
    const auto it = lightTypeMap.find(lightTypeString);
    RFX_CHECK_STATE(it != lightTypeMap.end(), "Unknown light type: " + lightTypeString);

    switch (it->second) {
    case LightType::DIRECTIONAL:
        return deserializeDirectionalLight(jsonLight);
        
    case LightType::POINT:
        return deserializePointLight(jsonLight);

    case LightType::SPOT:
        return deserializeSpotLight(jsonLight);

    default:
        RFX_THROW("Unknown light type: " + to_string(jsonLight["type"].asInt()));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Light> LightDefinitionDeserializer::deserializeDirectionalLight(const Json::Value& jsonLight) const
{
    auto light = make_shared<DirectionalLight>(jsonLight["id"].asString());

    deserializeCommonLightAttributes(jsonLight, light);

    Json::Value direction = jsonLight["direction"];
    light->setDirection(direction[0].asFloat(), direction[1].asFloat(), direction[2].asFloat());

    return light;
}

// ---------------------------------------------------------------------------------------------------------------------

void LightDefinitionDeserializer::deserializeCommonLightAttributes(
    const Json::Value& jsonLight,
    const shared_ptr<Light>& light) const
{
    Json::Value ambient = jsonLight["ambient"];
    light->setAmbient(ambient[0].asFloat(), ambient[1].asFloat(), ambient[2].asFloat());

    Json::Value diffuse = jsonLight["diffuse"];
    light->setDiffuse(diffuse[0].asFloat(), diffuse[1].asFloat(), diffuse[2].asFloat());

    Json::Value specular = jsonLight["specular"];
    light->setSpecular(specular[0].asFloat(), specular[1].asFloat(), specular[2].asFloat());
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Light> LightDefinitionDeserializer::deserializePointLight(const Json::Value& jsonLight) const
{
    shared_ptr<PointLight> light = make_unique<PointLight>(jsonLight["id"].asString());

    deserializeCommonLightAttributes(jsonLight, light);

    Json::Value position = jsonLight["position"];
    light->setPosition(position[0].asFloat(), position[1].asFloat(), position[2].asFloat());

    return light;
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Light> LightDefinitionDeserializer::deserializeSpotLight(const Json::Value& jsonLight) const
{
    shared_ptr<SpotLight> light = make_unique<SpotLight>(jsonLight["id"].asString());

    deserializeCommonLightAttributes(jsonLight, light);

    Json::Value position = jsonLight["position"];
    light->setPosition(position[0].asFloat(), position[1].asFloat(), position[2].asFloat());

    Json::Value direction = jsonLight["direction"];
    light->setDirection(direction[0].asFloat(), direction[1].asFloat(), direction[2].asFloat());

    light->setCutoff(jsonLight["spotCutoff"].asFloat());
    light->setExponent(jsonLight["spotExponent"].asFloat());

    return light;
}

// ---------------------------------------------------------------------------------------------------------------------
