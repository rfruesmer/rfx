#include "rfx/pch.h"
#include "rfx/scene/Effect.h"
#include "rfx/graphics/ShaderLoader.h"
#include "rfx/common/Logger.h"


using namespace rfx;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

Effect::Effect(shared_ptr<GraphicsDevice> graphicsDevice)
    : graphicsDevice_(move(graphicsDevice)) {}

// ---------------------------------------------------------------------------------------------------------------------

void Effect::loadShaders(const std::shared_ptr<Material>& material, const std::filesystem::path& shadersDirectory)
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

const shared_ptr<VertexShader>& Effect::getVertexShader() const
{
    return vertexShader;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<FragmentShader>& Effect::getFragmentShader() const
{
    return fragmentShader;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> Effect::buildShaderDefines(const shared_ptr<Material>& material, const VertexFormat& vertexFormat)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> Effect::buildVertexShaderInputs(const VertexFormat& vertexFormat)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> Effect::buildVertexShaderOutputs(const VertexFormat& vertexFormat)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> Effect::buildFragmentShaderInputs(const VertexFormat& vertexFormat)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

VertexFormat Effect::getVertexFormat() const
{
    return vertexShader->getVertexFormat();
}

// ---------------------------------------------------------------------------------------------------------------------
