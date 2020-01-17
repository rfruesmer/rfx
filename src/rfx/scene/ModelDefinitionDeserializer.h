#pragma once

#include "rfx/scene/ModelDefinition.h"

namespace rfx
{
    
class ModelDefinitionDeserializer
{
public:
    ModelDefinition deserialize(const Json::Value& jsonModel) const;
};

}
