#include "rfx/pch.h"
#include "rfx/core/JsonDeserializer.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

vec4 JsonDeserializer::loadVector4f(const Json::Value& jsonVector)
{
    return vec4(
        jsonVector[0].asFloat(), 
        jsonVector[1].asFloat(), 
        jsonVector[2].asFloat(), 
        jsonVector[3].asFloat()
    );
}

// ---------------------------------------------------------------------------------------------------------------------
