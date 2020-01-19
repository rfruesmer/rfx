#pragma once

#include "rfx/scene/ModelDefinition.h"

namespace rfx
{
    
class ModelDefinitionDeserializer
{
public:
    explicit ModelDefinitionDeserializer(std::unordered_map<std::string, EffectDefinition> effectDefaults);

    ModelDefinition deserialize(const Json::Value& jsonModel) const;

private:
    std::unordered_map<std::string, EffectDefinition> effectDefaults;
};

}
