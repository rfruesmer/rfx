#pragma once

namespace rfx
{

class JsonDeserializer
{
public:

protected:
    static glm::vec4 loadVector4f(const Json::Value& jsonVector);
};

} // namespace rfx
