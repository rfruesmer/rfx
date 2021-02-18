#pragma once

// ---------------------------------------------------------------------------------------------------------------------

namespace rfx
{
    void GLSLtoSPV(VkShaderStageFlagBits shaderType, const char* shaderString, std::vector<unsigned int>& spirv);
} // namespace rfx

// ---------------------------------------------------------------------------------------------------------------------