#include "rfx/pch.h"
#include "rfx/scene/ModelDefinition.h"

using namespace rfx;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

ModelDefinition::ModelDefinition(
    const path& modelPath,
    const VertexFormat& vertexFormat,
    const path& vertexShaderPath,
    const path& fragmentShaderPath,
    const path& texturePath)
        : modelPath(modelPath),
          vertexFormat(vertexFormat),
          vertexShaderPath(vertexShaderPath),
          fragmentShaderPath(fragmentShaderPath),
          texturePath(texturePath) {}

// ---------------------------------------------------------------------------------------------------------------------

void ModelDefinition::setModelPath(const path& path)
{
    this->modelPath = path;
}

// ---------------------------------------------------------------------------------------------------------------------

const path& ModelDefinition::getModelPath() const
{
    return modelPath;
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelDefinition::setVertexFormat(const VertexFormat& format)
{
    this->vertexFormat = format;
}

// ---------------------------------------------------------------------------------------------------------------------

const VertexFormat& ModelDefinition::getVertexFormat() const
{
    return vertexFormat;
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelDefinition::setVertexShaderPath(const path& path)
{
    this->vertexShaderPath = path;
}

// ---------------------------------------------------------------------------------------------------------------------

const path& ModelDefinition::getVertexShaderPath() const
{
    return vertexShaderPath;
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelDefinition::setFragmentShaderPath(const path& path)
{
    this->fragmentShaderPath = path;
}

// ---------------------------------------------------------------------------------------------------------------------

const path& ModelDefinition::getFragmentShaderPath() const
{
    return fragmentShaderPath;
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelDefinition::setTexturePath(const path& path)
{
    texturePath = path;
}

// ---------------------------------------------------------------------------------------------------------------------

const path& ModelDefinition::getTexturePath() const
{
    return texturePath;
}

// ---------------------------------------------------------------------------------------------------------------------
