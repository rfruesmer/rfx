#include "rfx/pch.h"
#include "rfx/graphics/effect/EffectDefinitionDeserializer.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

EffectDefinition EffectDefinitionDeserializer::deserialize(const Json::Value& jsonEffect) const
{
    return deserializeInternal(jsonEffect, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

EffectDefinition EffectDefinitionDeserializer::deserialize(const Json::Value& jsonEffect,
    const EffectDefinition& defaultValues) const
{
    return deserializeInternal(jsonEffect, &defaultValues);
}

// ---------------------------------------------------------------------------------------------------------------------

EffectDefinition EffectDefinitionDeserializer::deserializeInternal(const Json::Value& jsonEffect,
    const EffectDefinition* defaultValues) const
{
    EffectDefinition effectDefinition;
    effectDefinition.id = jsonEffect["id"].asString();

    const Json::Value jsonVertexFormat = jsonEffect["vertexFormat"];
    if (!jsonVertexFormat.empty()) {
        unsigned int vertexFormatBits = 0;
        for (const auto& it : jsonVertexFormat) {
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

        effectDefinition.vertexFormat = VertexFormat(vertexFormatBits);
    }
    else if (defaultValues) {
        effectDefinition.vertexFormat = defaultValues->vertexFormat;
    }

    if (!jsonEffect["vertexShaderPath"].empty()) {
        effectDefinition.vertexShaderPath = jsonEffect["vertexShaderPath"].asString();
    }
    else if (defaultValues) {
        effectDefinition.vertexShaderPath = defaultValues->vertexShaderPath;
    }

    if (!jsonEffect["fragmentShaderPath"].empty()) {
        effectDefinition.fragmentShaderPath = jsonEffect["fragmentShaderPath"].asString();
    }
    else if (defaultValues) {
        effectDefinition.fragmentShaderPath = defaultValues->fragmentShaderPath;
    }

    if (!jsonEffect["texturePaths"].empty()) {
        for (const Json::Value& jsonTexturePath : jsonEffect["texturePaths"]) {
            effectDefinition.texturePaths.push_back(jsonTexturePath.asString());
        }
    }
    else if (defaultValues) {
        effectDefinition.texturePaths = defaultValues->texturePaths;
    }

    return effectDefinition;

}

// ---------------------------------------------------------------------------------------------------------------------
