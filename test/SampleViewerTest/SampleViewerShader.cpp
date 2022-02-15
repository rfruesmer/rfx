#include "rfx/pch.h"
#include "SampleViewerShader.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

const string SampleViewerShader::ID = "sample_viewer";
static const string VERTEX_SHADER_ID = "pbr_gltf/pbr_gltf";
static const string FRAGMENT_SHADER_ID = "pbr_gltf/pbr_gltf";

// ---------------------------------------------------------------------------------------------------------------------

SampleViewerShader::SampleViewerShader(const GraphicsDevicePtr& graphicsDevice)
    : TestMaterialShader(
        graphicsDevice,
        ID,
        VERTEX_SHADER_ID,
        FRAGMENT_SHADER_ID) {}

// ---------------------------------------------------------------------------------------------------------------------

vector<std::byte> SampleViewerShader::createDataFor(const MaterialPtr& material) const
{
    const MaterialData materialData {
        .baseColorFactor = material->getBaseColorFactor()
    };

    vector<std::byte> data(sizeof(MaterialData));
    memcpy(data.data(), &materialData, sizeof(MaterialData));

    return data;
}

// ---------------------------------------------------------------------------------------------------------------------

const void* SampleViewerShader::getData() const
{
    return reinterpret_cast<const void*>(&data);
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t SampleViewerShader::getDataSize() const
{
    return sizeof(ShaderData);
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> SampleViewerShader::getShaderDefinesFor(const MaterialPtr& material)
{
    vector<string> defines;

    defines.emplace_back("MATERIAL_METALLICROUGHNESS");
    defines.emplace_back("USE_PUNCTUAL");
    defines.emplace_back("LINEAR_OUTPUT");

    if (material->getBaseColorTexture() != nullptr) {
        defines.emplace_back("HAS_BASE_COLOR_MAP 1");
    }

    const VertexFormat& vertexFormat = material->getVertexFormat();

    if (vertexFormat.containsColors3()) {
        defines.emplace_back("HAS_COLOR_VEC3");
    }
    else if (vertexFormat.containsColors4()) {
        defines.emplace_back("HAS_COLOR_VEC4");
    }

    if (vertexFormat.containsNormals()) {
        defines.emplace_back("HAS_NORMAL_VEC3");
    }

    if (vertexFormat.containsTexCoords()) {
        defines.emplace_back("HAS_TEXCOORD_VEC2");
    }

    if (vertexFormat.containsTangents()) {
        defines.emplace_back("HAS_TANGENTS 1");
    }

    return defines;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> SampleViewerShader::getVertexShaderInputsFor(const MaterialPtr& material)
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

    return inputs;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> SampleViewerShader::getVertexShaderOutputsFor(const MaterialPtr& material)
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

    return outputs;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> SampleViewerShader::getFragmentShaderInputsFor(const MaterialPtr& material)
{
    vector<string> inputs;

    // location=0 for coordinates (must be always present)
    uint32_t location = 1;

    const VertexFormat& vertexFormat = material->getVertexFormat();

    if (vertexFormat.containsColors3()) {
        inputs.push_back(fmt::format("layout(location = {}) in vec3 inColor;", location));
        location++;
    }
    else if (vertexFormat.containsColors4()) {
        inputs.push_back(fmt::format("layout(location = {}) in vec4 inColor;", location));
        location++;
    }

    if (vertexFormat.containsNormals()) {
        inputs.push_back(fmt::format("layout(location = {}) in vec3 inNormal;", location));
        location++;
    }

    uint32_t texCoordSetCount = vertexFormat.getTexCoordSetCount();
    if (texCoordSetCount > 0) {
        inputs.push_back(fmt::format("layout(location = {}) in vec2 inTexCoord[{}];", location, texCoordSetCount));
        location += texCoordSetCount;
    }

    return inputs;
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerShader::setLight(size_t index, const LightPtr& light)
{
    RFX_CHECK_ARGUMENT(index < MAX_LIGHTS);

    if (light == nullptr || !light->isEnabled()) {
        data.lights[index].enabled = false;
        return;
    }

    auto& lightData = data.lights[index];
    lightData.type = light->getType();
    lightData.enabled = true;
    lightData.color = light->getColor();

    switch (light->getType())
    {
    case Light::DIRECTIONAL:
        setDirectionalLight(static_pointer_cast<DirectionalLight>(light), &lightData);
        break;

    case Light::POINT:
        RFX_THROW_NOT_IMPLEMENTED();
        break;

    case Light::SPOT:
        RFX_THROW_NOT_IMPLEMENTED();
        break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerShader::setDirectionalLight(
    const DirectionalLightPtr& light,
    LightData* outLightData)
{
    outLightData->direction = light->getDirection();
}

// ---------------------------------------------------------------------------------------------------------------------
