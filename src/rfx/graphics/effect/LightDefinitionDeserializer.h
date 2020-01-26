#pragma once

#include "rfx/graphics/effect/Light.h"

namespace rfx
{
    
class LightDefinitionDeserializer
{
public:
    [[nodiscard]] std::shared_ptr<Light> deserialize(const Json::Value& jsonLight) const;

private:
    [[nodiscard]] std::shared_ptr<Light> deserializeDirectionalLight(const Json::Value& jsonLight) const;
    [[nodiscard]] std::shared_ptr<Light> deserializePointLight(const Json::Value& jsonLight) const;
    [[nodiscard]] std::shared_ptr<Light> deserializeSpotLight(const Json::Value& jsonLight) const;

    void deserializeCommonLightAttributes(const Json::Value& jsonLight, const std::shared_ptr<Light>& light) const;
};

}
