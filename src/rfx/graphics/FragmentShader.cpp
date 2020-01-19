#include "rfx/pch.h"
#include "rfx/graphics/FragmentShader.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

FragmentShader::FragmentShader(const shared_ptr<GraphicsDevice>& graphicsDevice, 
    const VkPipelineShaderStageCreateInfo& stageCreateInfo)
        : Shader(graphicsDevice, stageCreateInfo) {}

// ---------------------------------------------------------------------------------------------------------------------
