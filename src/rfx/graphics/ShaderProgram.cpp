#include "rfx/pch.h"
#include "rfx/graphics/ShaderProgram.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

ShaderProgram::ShaderProgram(
    VertexShaderPtr vertexShader,
    FragmentShaderPtr fragmentShader)
        : vertexShader(move(vertexShader)),
          fragmentShader(move(fragmentShader)) {}

// ---------------------------------------------------------------------------------------------------------------------

const VertexShaderPtr& ShaderProgram::getVertexShader() const
{
    return vertexShader;
}

// ---------------------------------------------------------------------------------------------------------------------

const FragmentShaderPtr& ShaderProgram::getFragmentShader() const
{
    return fragmentShader;
}

// ---------------------------------------------------------------------------------------------------------------------
