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
        .baseColor = material->getBaseColorFactor()
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

    if (vertexFormat.containsTangents()) {
        inputs.push_back(fmt::format("layout(location = {}) in vec4 inTangent;", location));
        location++;
    }

    return inputs;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> SampleViewerShader::getVertexShaderOutputsFor(const MaterialPtr& material)
{
    return MaterialShader::getVertexShaderOutputsFor(material);
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> SampleViewerShader::getFragmentShaderInputsFor(const MaterialPtr& material)
{
    return MaterialShader::getFragmentShaderInputsFor(material);
}

// ---------------------------------------------------------------------------------------------------------------------
