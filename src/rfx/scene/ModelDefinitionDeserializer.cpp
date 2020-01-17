#include "rfx/pch.h"
#include "rfx/scene/ModelDefinitionDeserializer.h"

using namespace rfx;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

ModelDefinition ModelDefinitionDeserializer::deserialize(const Json::Value& jsonModel) const
{
    unsigned int vertexFormatBits = 0;

    for (const auto& it : jsonModel["vertexFormat"]) {
        string currentFormatComponent = it.asString();
        if (currentFormatComponent == "COORDINATES") {
            vertexFormatBits |= VertexFormat::COORDINATES;
        }
        else if (currentFormatComponent == "COLORS") {
            vertexFormatBits |= VertexFormat::COLORS;
        }
        else if (currentFormatComponent == "TEXCOORDS") {
            vertexFormatBits |= VertexFormat::TEXCOORDS;
        }
        else {
            RFX_THROW("Unknown vertex format component: " + currentFormatComponent);
        }
    }

    return ModelDefinition(
        current_path() / jsonModel["path"].asString(),
        VertexFormat(vertexFormatBits), 
        current_path() / jsonModel["vertexShaderPath"].asString(), 
        current_path() / jsonModel["fragmentShaderPath"].asString());
}

// ---------------------------------------------------------------------------------------------------------------------
