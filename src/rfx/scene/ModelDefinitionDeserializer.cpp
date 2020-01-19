#include "rfx/pch.h"
#include "rfx/scene/ModelDefinitionDeserializer.h"
#include "rfx/graphics/effect/EffectDefinitionDeserializer.h"

using namespace rfx;
using namespace glm;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

ModelDefinitionDeserializer::ModelDefinitionDeserializer(
    unordered_map<string, EffectDefinition> effectDefaults)
        : effectDefaults(effectDefaults) {}

// ---------------------------------------------------------------------------------------------------------------------

ModelDefinition ModelDefinitionDeserializer::deserialize(const Json::Value& jsonModel) const
{
    EffectDefinitionDeserializer effectDefinitionDeserializer;
    EffectDefinition effectDefinition;

    const Json::Value& jsonEffect = jsonModel["effect"];
    const auto& it = effectDefaults.find(jsonEffect["id"].asString());
    effectDefinition = it != effectDefaults.end()
        ? effectDefinitionDeserializer.deserialize(jsonEffect, it->second)
        : effectDefinitionDeserializer.deserialize(jsonEffect);

    Transform transform;

    const Json::Value jsonTransform = jsonModel["transform"];
    if (jsonTransform) {
        Json::Value currentValue = jsonTransform["translation"];
        vec3 translation = {
            currentValue[0].asFloat(), 
            currentValue[1].asFloat(), 
            currentValue[2].asFloat()
        };
        if (length(translation) != 0.0F) {
            transform.setTranslation(translation);
        }

        currentValue = jsonTransform["scale"];
        vec3 scale = {
            currentValue[0].asFloat(), 
            currentValue[1].asFloat(), 
            currentValue[2].asFloat()
        };
        if (length(scale) != 1.0F) {
            transform.setScale(scale);
        }

        currentValue = jsonTransform["rotation"];
        vec3 rotation = {
            currentValue[0].asFloat(),
            currentValue[1].asFloat(),
            currentValue[2].asFloat()
        };

        if (length(rotation) != 0.0F) {
            transform.setRotation(rotation);
        }
    }

    return ModelDefinition {
        jsonModel["path"].asString(),
        effectDefinition, 
        transform
    };
}

// ---------------------------------------------------------------------------------------------------------------------
