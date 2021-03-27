#include "rfx/pch.h"
#include "rfx/scene/MaterialShader.h"
#include "rfx/graphics/ShaderLoader.h"
#include "rfx/common/Logger.h"


using namespace rfx;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

MaterialShader::MaterialShader(shared_ptr<GraphicsDevice> graphicsDevice)
    : graphicsDevice_(move(graphicsDevice)) {}

// ---------------------------------------------------------------------------------------------------------------------

void MaterialShader::loadShaders(const std::shared_ptr<Material>& material, const std::filesystem::path& shadersDirectory)
{
    const path vertexShaderFilename = material->getVertexShaderId() + ".vert";
    const path fragmentShaderFilename = material->getFragmentShaderId() + ".frag";
    const VertexFormat& vertexFormat = material->getVertexFormat();

    const vector<string> defines = buildShaderDefines(material, vertexFormat);
    const vector<string> vertexShaderInputs = buildVertexShaderInputs(vertexFormat);
    const vector<string> vertexShaderOutputs = buildVertexShaderOutputs(vertexFormat);
    const vector<string> fragmentShaderInputs = buildFragmentShaderInputs(vertexFormat);


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

const shared_ptr<VertexShader>& MaterialShader::getVertexShader() const
{
    return vertexShader;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<FragmentShader>& MaterialShader::getFragmentShader() const
{
    return fragmentShader;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> MaterialShader::buildShaderDefines(const shared_ptr<Material>& material, const VertexFormat& vertexFormat)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> MaterialShader::buildVertexShaderInputs(const VertexFormat& vertexFormat)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> MaterialShader::buildVertexShaderOutputs(const VertexFormat& vertexFormat)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> MaterialShader::buildFragmentShaderInputs(const VertexFormat& vertexFormat)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------
