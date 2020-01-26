#pragma once

#include "rfx/graphics/VertexFormat.h"
#include "rfx/graphics/effect/Material.h"

namespace rfx
{

class EffectDefinition
{
public:
    std::string id;
    VertexFormat vertexFormat;
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
    Material::Data material;
    std::vector<std::string> texturePaths;
};
    
}
