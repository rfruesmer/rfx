#include "rfx/pch.h"
#include "rfx/scene/ModelDefinitionDeserializer.h"

using namespace rfx;
using namespace glm;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

ModelDefinition ModelDefinitionDeserializer::deserialize(const Json::Value& jsonModel) const
{
    Transform transform;

    const Json::Value jsonTransform = jsonModel["transform"];
    if (jsonTransform) {
        Json::Value currentValue = jsonTransform["translation"];
        vec3 translation = {
            currentValue[0].asFloat(), 
            currentValue[1].asFloat(), 
            currentValue[2].asFloat()
        };
        if (length(translation) != 0.0F) {
            transform.setTranslation(translation);
        }

        currentValue = jsonTransform["scale"];
        vec3 scale = {
            currentValue[0].asFloat(), 
            currentValue[1].asFloat(), 
            currentValue[2].asFloat()
        };
        if (length(scale) != 1.0F) {
            transform.setScale(scale);
        }

        currentValue = jsonTransform["rotation"];
        vec3 rotation = {
            currentValue[0].asFloat(),
            currentValue[1].asFloat(),
            currentValue[2].asFloat()
        };

        if (length(rotation) != 0.0F) {
            transform.setRotation(rotation);
        }
    }

    const Json::Value& jsonEffect = jsonModel["effect"];

    unsigned int vertexFormatBits = 0;

    for (const auto& it : jsonEffect["vertexFormat"]) {
        auto currentFormatComponent = it.asString();
        if (currentFormatComponent == "COORDINATES") {
            vertexFormatBits |= VertexFormat::COORDINATES;
        }
        else if (currentFormatComponent == "COLORS") {
            vertexFormatBits |= VertexFormat::COLORS;
        }
        else if (currentFormatComponent == "NORMALS") {
            vertexFormatBits |= VertexFormat::NORMALS;
        }
        else if (currentFormatComponent == "TEXCOORDS") {
            vertexFormatBits |= VertexFormat::TEXCOORDS;
        }
        else {
            RFX_THROW("Unknown vertex format component: " + currentFormatComponent);
        }
    }

    EffectDefinition effect;
    effect.id = jsonEffect["id"].asString();
    effect.vertexFormat = VertexFormat(vertexFormatBits);
    effect.vertexShaderPath = jsonEffect["vertexShaderPath"].asString();
    effect.fragmentShaderPath = jsonEffect["fragmentShaderPath"].asString();

    for (const Json::Value& jsonTexturePath : jsonEffect["texturePaths"]) {
        effect.texturePaths.push_back(jsonTexturePath.asString());
    }

    return ModelDefinition {
        jsonModel["path"].asString(),
        effect, 
        transform
    };
}

// ---------------------------------------------------------------------------------------------------------------------
