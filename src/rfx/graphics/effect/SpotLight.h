#pragma once

#include "rfx/graphics/effect/Light.h"

namespace rfx
{

class SpotLight : public Light
{
public:
    explicit SpotLight(const std::string& id);
};

}
