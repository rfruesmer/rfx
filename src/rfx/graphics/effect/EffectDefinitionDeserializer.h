#pragma once

#include "rfx/graphics/effect/EffectDefinition.h"
#include "rfx/core/JsonDeserializer.h"

namespace rfx
{

class EffectDefinitionDeserializer : public JsonDeserializer
{
public:
    EffectDefinition deserialize(const Json::Value& jsonEffect) const;
    EffectDefinition deserialize(const Json::Value& jsonEffect, 
        const EffectDefinition& defaultValues) const;

private:
    EffectDefinition deserializeInternal(const Json::Value& jsonEffect, 
        const EffectDefinition* defaultValues) const;
    void deserializeVertexFormat(const Json::Value& jsonEffect,
        const EffectDefinition* defaultValues,
        EffectDefinition& effectDefinition) const;
    void deserializeShaders(const Json::Value& jsonEffect,
        const EffectDefinition* defaultValues,
        EffectDefinition& effectDefinition) const;
    void deserializeMaterial(const Json::Value& jsonEffect,
        const EffectDefinition* defaultValues,
        EffectDefinition& effectDefinition) const;
    void deserializeTexturePaths(const Json::Value& jsonEffect,
        const EffectDefinition* defaultValues,
        EffectDefinition& effectDefinition) const;
};
    
} // namespace rfx
