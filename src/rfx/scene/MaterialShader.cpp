#include "rfx/pch.h"
#include "rfx/scene/MaterialShader.h"
#include "rfx/graphics/ShaderLoader.h"
#include "rfx/common/Logger.h"


using namespace rfx;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

MaterialShader::MaterialShader(
    GraphicsDevicePtr graphicsDevice,
    std::string id,
    std::string vertexShaderId,
    std::string fragmentShaderId)
    : graphicsDevice_(move(graphicsDevice)),
      id(move(id)),
      vertexShaderId(move(vertexShaderId)),
      fragmentShaderId(move(fragmentShaderId)) {}

// ---------------------------------------------------------------------------------------------------------------------

MaterialShader::~MaterialShader()
{
    int i = 42;
}

// ---------------------------------------------------------------------------------------------------------------------

const string& MaterialShader::getId() const
{
    return id;
}

// ---------------------------------------------------------------------------------------------------------------------

// TODO: move to factory (?!)
void MaterialShader::loadShaders(
    const std::shared_ptr<Material>& material,
    const std::filesystem::path& shadersDirectory)
{
    const path vertexShaderFilename = vertexShaderId + ".vert";
    const path fragmentShaderFilename = fragmentShaderId + ".frag";
    const VertexFormat& vertexFormat = material->getVertexFormat();

    const vector<string> defines = getShaderDefinesFor(material);
    const vector<string> vertexShaderInputs = getVertexShaderInputsFor(material);
    const vector<string> vertexShaderOutputs = getVertexShaderOutputsFor(material);
    const vector<string> fragmentShaderInputs = getFragmentShaderInputsFor(material);


    const ShaderLoader shaderLoader(graphicsDevice_);
    RFX_LOG_INFO << "Loading vertex shader ...";
    vertexShader = shaderLoader.loadVertexShader(
        shadersDirectory / vertexShaderFilename,
        "main",
        vertexFormat,
        defines,
        vertexShaderInputs,
        vertexShaderOutputs);

    RFX_LOG_INFO << "Loading fragment shader ...";
    fragmentShader = shaderLoader.loadFragmentShader(
        shadersDirectory / fragmentShaderFilename,
        "main",
        defines,
        fragmentShaderInputs);
}

// ---------------------------------------------------------------------------------------------------------------------

const string& MaterialShader::getVertexShaderId() const
{
    return vertexShaderId;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<VertexShader>& MaterialShader::getVertexShader() const
{
    return vertexShader;
}

// ---------------------------------------------------------------------------------------------------------------------

const string& MaterialShader::getFragmentShaderId() const
{
    return fragmentShaderId;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<FragmentShader>& MaterialShader::getFragmentShader() const
{
    return fragmentShader;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> MaterialShader::getShaderDefinesFor(const MaterialPtr& material)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> MaterialShader::getVertexShaderInputsFor(const MaterialPtr& material)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> MaterialShader::getVertexShaderOutputsFor(const MaterialPtr& material)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> MaterialShader::getFragmentShaderInputsFor(const MaterialPtr& material)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------
