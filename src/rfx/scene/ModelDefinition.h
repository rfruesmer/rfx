#pragma once

#include "rfx/scene/Transform.h"
#include "rfx/graphics/EffectDefinition.h"

namespace rfx
{

struct ModelDefinition
{
    std::filesystem::path modelPath;
    EffectDefinition effect;
    Transform transform;
};    

} // namespace rfx
