#pragma once

#include "rfx/graphics/effect/Light.h"

namespace rfx
{

class PointLight : public Light
{
public:
    explicit PointLight(const std::string& id);
};
    
}
