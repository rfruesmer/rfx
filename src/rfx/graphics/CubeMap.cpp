#include "rfx/pch.h"
#include "rfx/graphics/CubeMap.h"


using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

CubeMap::CubeMap(
    VkDevice device,
    ImagePtr image,
    VkImageView imageView,
    VkImageLayout imageLayout,
    VkSampler sampler)
        : Texture(
            device,
            image,
            imageView,
            imageLayout,
            sampler) {}

// ---------------------------------------------------------------------------------------------------------------------
