#pragma once

// ---------------------------------------------------------------------------------------------------------------------

namespace rfx
{
    void GLSLtoSPV(VkShaderStageFlagBits shaderType, const char* shaderString, std::vector<uint32_t>& spirv);
} // namespace rfx

// ---------------------------------------------------------------------------------------------------------------------