#pragma once


namespace rfx {

struct SamplerDesc {
    VkFilter minFilter = VK_FILTER_LINEAR;
    VkFilter magFilter = VK_FILTER_LINEAR;
    VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    float maxLod = 1.0f;
};

} // namespace rfx