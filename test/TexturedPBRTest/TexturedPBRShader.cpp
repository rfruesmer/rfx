#include "rfx/pch.h"
#include "TexturedPBRShader.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

const string TexturedPBRShader::ID = "pbr_textured";

// ---------------------------------------------------------------------------------------------------------------------

TexturedPBRShader::TexturedPBRShader(GraphicsDevicePtr& graphicsDevice)
    : TestMaterialShader(
        graphicsDevice,
        ID,
        ID,
        ID) {}

// ---------------------------------------------------------------------------------------------------------------------

vector<std::byte> TexturedPBRShader::createDataFor(const MaterialPtr& material) const
{
    const MaterialData materialData {
        .baseColorFactor = material->getBaseColorFactor(),
        .emissiveFactor = vec4(material->getEmissiveFactor(), 1.0f),
        .metallic = material->getMetallicFactor(),
        .roughness = material->getRoughnessFactor(),
        .baseColorTexCoordSet = material->getBaseColorTexCoordSet(),
        .metallicRoughnessTexCoordSet = material->getMetallicRoughnessTexCoordSet(),
        .normalTexCoordSet = material->getNormalTexCoordSet(),
        .occlusionTexCoordSet = material->getOcclusionTexCoordSet(),
        .occlusionStrength = material->getOcclusionStrength(),
        .emissiveTexCoordSet = material->getEmissiveTexCoordSet()
    };


    vector<std::byte> data(sizeof(MaterialData));
    memcpy(data.data(), &materialData, sizeof(MaterialData));

    return data;
}

// ---------------------------------------------------------------------------------------------------------------------

const void* TexturedPBRShader::getData() const
{
    return reinterpret_cast<const void*>(&data);
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t TexturedPBRShader::getDataSize() const
{
    return sizeof(ShaderData);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBRShader::setLight(int index, const PointLightPtr& light)
{
    if (light == nullptr) {
        data.lights[index].enabled = false;
        return;
    }

    auto& lightData = data.lights[index];
    lightData.enabled = true;
    lightData.position = light->getPosition();
    lightData.color = vec4(light->getColor() * 255.0f, 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> TexturedPBRShader::getShaderDefinesFor(const MaterialPtr& material)
{
    vector<string> defines;

    if (material->getBaseColorTexture() != nullptr) {
        defines.emplace_back("HAS_BASE_COLOR_MAP 1");
    }

    if (material->getMetallicRoughnessTexture() != nullptr) {
        defines.emplace_back("HAS_METALLIC_ROUGHNESS_MAP 1");
    }

    if (material->getNormalTexture() != nullptr) {
        defines.emplace_back("HAS_NORMAL_MAP 1");
    }

    if (material->getOcclusionTexture() != nullptr) {
        defines.emplace_back("HAS_OCCLUSION_MAP 1");
    }

    if (material->getEmissiveTexture() != nullptr) {
        defines.emplace_back("HAS_EMISSIVE_MAP 1");
    }

    const VertexFormat& vertexFormat = material->getVertexFormat();
    if (vertexFormat.containsNormals()) {
        defines.emplace_back("HAS_NORMALS 1");
    }

    const uint32_t texCoordSetCount = vertexFormat.getTexCoordSetCount();
    if (texCoordSetCount > 0) {
        defines.push_back(fmt::format("TEXCOORDSET_COUNT {}", texCoordSetCount));
    }

    if (vertexFormat.containsTangents()) {
        defines.emplace_back("HAS_TANGENTS 1");
    }

    return defines;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> TexturedPBRShader::getVertexShaderInputsFor(const MaterialPtr& material)
{
    vector<string> inputs;

    // location=0 for coordinates (must be present always)
    uint32_t location = 1;

    const VertexFormat& vertexFormat = material->getVertexFormat();
    if (vertexFormat.containsNormals()) {
        inputs.push_back(fmt::format("layout(location = {}) in vec3 inNormal;", location));
        location++;
    }

    uint32_t texCoordSetCount = vertexFormat.getTexCoordSetCount();
    if (texCoordSetCount > 0) {
        inputs.push_back(fmt::format("layout(location = {}) in vec2 inTexCoord[{}];", location, texCoordSetCount));
        location += texCoordSetCount;
    }

    if (vertexFormat.containsTangents()) {
        inputs.push_back(fmt::format("layout(location = {}) in vec4 inTangent;", location));
        location++;
    }

    return inputs;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> TexturedPBRShader::getVertexShaderOutputsFor(const MaterialPtr& material)
{
    vector<string> outputs;

    // location=0 for coordinates (must be always present)
    uint32_t location = 1;

    const VertexFormat& vertexFormat = material->getVertexFormat();
    if (vertexFormat.containsNormals()) {
        outputs.push_back(fmt::format("layout(location = {}) out vec3 outNormal;", location));
        location++;
    }

    uint32_t texCoordSetCount = vertexFormat.getTexCoordSetCount();
    if (texCoordSetCount > 0) {
        outputs.push_back(fmt::format("layout(location = {}) out vec2 outTexCoord[{}];", location, texCoordSetCount));
        location += texCoordSetCount;
    }

    if (vertexFormat.containsTangents()) {
        outputs.push_back(fmt::format("layout(location = {}) out vec3 outTangentCamPos;", location++));
        outputs.push_back(fmt::format("layout(location = {}) out vec3 outTangentPosition;", location++));
        outputs.push_back(fmt::format("layout(location = {}) out vec3 outTangentLightPos[{}];", location, MAX_LIGHTS));
        location += MAX_LIGHTS;
    }

    return outputs;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> TexturedPBRShader::getFragmentShaderInputsFor(const MaterialPtr& material)
{
    vector<string> inputs;

    // location=0 for coordinates (must be always present)
    uint32_t location = 1;

    const VertexFormat& vertexFormat = material->getVertexFormat();
    if (vertexFormat.containsNormals()) {
        inputs.push_back(fmt::format("layout(location = {}) in vec3 inNormal;", location));
        location++;
    }

    uint32_t texCoordSetCount = vertexFormat.getTexCoordSetCount();
    if (texCoordSetCount > 0) {
        inputs.push_back(fmt::format("layout(location = {}) in vec2 inTexCoord[{}];", location, texCoordSetCount));
        location += texCoordSetCount;
    }

    if (vertexFormat.containsTangents()) {
        inputs.push_back(fmt::format("layout(location = {}) in vec3 inTangentCamPos;", location));
        location++;
        inputs.push_back(fmt::format("layout(location = {}) in vec3 inTangentPosition;", location));
        location++;
        inputs.push_back(fmt::format("layout(location = {}) in vec3 inTangentLightPos[{}];", location, MAX_LIGHTS));
        location += MAX_LIGHTS;
    }

    return inputs;
}

// ---------------------------------------------------------------------------------------------------------------------
