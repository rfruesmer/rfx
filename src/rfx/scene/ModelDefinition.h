#pragma once

#include "rfx/scene/Transform.h"
#include "rfx/graphics/VertexFormat.h"

namespace rfx
{

struct ModelDefinition
{
    std::filesystem::path modelPath;
    VertexFormat vertexFormat;
    std::filesystem::path vertexShaderPath;
    std::filesystem::path fragmentShaderPath;
    std::filesystem::path texturePath;
    Transform transform;
};    

} // namespace rfx
