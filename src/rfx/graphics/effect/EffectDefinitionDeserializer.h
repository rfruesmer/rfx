#pragma once

#include "rfx/graphics/effect/EffectDefinition.h"

namespace rfx
{

class EffectDefinitionDeserializer
{
public:
    EffectDefinition deserialize(const Json::Value& jsonEffect) const;
    EffectDefinition deserialize(const Json::Value& jsonEffect, const EffectDefinition& defaultValues) const;

private:
    EffectDefinition deserializeInternal(const Json::Value& jsonEffect, const EffectDefinition* defaultValues) const;
};
    
} // namespace rfx
