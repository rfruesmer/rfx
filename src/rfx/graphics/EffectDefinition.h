#pragma once

#include "rfx/graphics/VertexFormat.h"

namespace rfx
{

class EffectDefinition
{
public:
    std::string id;
    VertexFormat vertexFormat;
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
    std::vector<std::string> texturePaths;
};
    
}
