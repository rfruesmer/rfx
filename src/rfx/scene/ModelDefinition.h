#pragma once

#include "rfx/scene/Transform.h"
#include "rfx/graphics/effect/EffectDefinition.h"

namespace rfx
{

struct ModelDefinition
{
    std::filesystem::path modelPath;
    EffectDefinition effect;
    Transform transform;
};    

} // namespace rfx
