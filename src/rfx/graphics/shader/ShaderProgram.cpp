#include "rfx/pch.h"
#include "rfx/graphics/shader/ShaderProgram.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

ShaderProgram::ShaderProgram(const shared_ptr<VertexShader>& vertexShader, 
    const shared_ptr<FragmentShader>& fragmentShader)
        : vertexShader(vertexShader),
          fragmentShader(fragmentShader)
{
    shaderStages.push_back(vertexShader->getStageCreateInfo());
    shaderStages.push_back(fragmentShader->getStageCreateInfo());
}

// ---------------------------------------------------------------------------------------------------------------------

const VertexFormat& ShaderProgram::getVertexFormat() const
{
    return vertexShader->getVertexFormat();
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<VertexShader>& ShaderProgram::getVertexShader() const
{
    return vertexShader;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<FragmentShader>& ShaderProgram::getFragmentShader() const
{
    return fragmentShader;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<VkPipelineShaderStageCreateInfo>& ShaderProgram::getShaderStages() const
{
    return shaderStages;
}

// ---------------------------------------------------------------------------------------------------------------------
