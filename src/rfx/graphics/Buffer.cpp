#include "rfx/pch.h"
#include "rfx/graphics/Buffer.h"


using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Buffer::Buffer(
    VkDeviceSize size,
    VkDevice device,
    VkBuffer buffer,
    VkDeviceMemory deviceMemory)
        : size_(size),
          device_(device),
          buffer_(buffer),
          deviceMemory_(deviceMemory)
{
    // TODO: support for sub-buffer allocation
    descriptorBufferInfo_ = {
        .buffer = buffer,
        .offset = 0,
        .range = VK_WHOLE_SIZE
    };
}

// ---------------------------------------------------------------------------------------------------------------------

Buffer::~Buffer()
{
    vkFreeMemory(device_, deviceMemory_, nullptr);
    vkDestroyBuffer(device_, buffer_, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void Buffer::load(size_t size, const void* data) const
{
    RFX_CHECK_ARGUMENT(size <= size_);

    uint8_t* mappedMemory = nullptr;
    ThrowIfFailed(vkMapMemory(
        device_,
        deviceMemory_,
        0,
        size,
        0,
        reinterpret_cast<void**>(&mappedMemory)));

    memcpy(mappedMemory, data, size);

    vkUnmapMemory(device_, deviceMemory_);
}

// ---------------------------------------------------------------------------------------------------------------------

VkBuffer Buffer::getHandle() const
{
    return buffer_;
}

// ---------------------------------------------------------------------------------------------------------------------

VkDeviceMemory Buffer::getDeviceMemory() const
{
    return deviceMemory_;
}

// ---------------------------------------------------------------------------------------------------------------------

VkDeviceSize Buffer::getSize() const
{
    return size_;
}

// ---------------------------------------------------------------------------------------------------------------------

const VkDescriptorBufferInfo& Buffer::getDescriptorBufferInfo() const
{
    return descriptorBufferInfo_;
}

// ---------------------------------------------------------------------------------------------------------------------
