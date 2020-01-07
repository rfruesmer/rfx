#include "rfx/pch.h"
#include "rfx/graphics/Buffer.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Buffer::Buffer(VkDevice vkDevice, 
    VkBuffer vkBuffer, 
    VkDeviceMemory vkDeviceMemory,
    const VulkanDeviceFunctionPtrs& vk,
    size_t size,
    size_t range
    )
        : vkDevice(vkDevice),
          vkBuffer(vkBuffer),
          vkDeviceMemory(vkDeviceMemory),
          size(size)
{
    vkMapMemory = vk.vkMapMemory;
    vkUnmapMemory = vk.vkUnmapMemory;
    vkBindBufferMemory = vk.vkBindBufferMemory;

    vkDestroyBuffer = vk.vkDestroyBuffer;
    vkFreeMemory = vk.vkFreeMemory;

    vkBufferInfo.buffer = vkBuffer;
    vkBufferInfo.offset = 0;
    vkBufferInfo.range = range;
}

// ---------------------------------------------------------------------------------------------------------------------

void Buffer::load(size_t size, const std::byte* data) const
{
    RFX_CHECK_ARGUMENT(size <= this->size, "invalid size");

    uint8_t* memory = nullptr;
    VkResult result = vkMapMemory(vkDevice, vkDeviceMemory, 0, this->size, 0, reinterpret_cast<void**>(&memory));
    RFX_CHECK_STATE(result == VK_SUCCESS && memory != nullptr,
        "Failed to map buffer memory");

    memcpy(memory, data, size);

    vkUnmapMemory(vkDevice, vkDeviceMemory);
}

// ---------------------------------------------------------------------------------------------------------------------

void Buffer::bind() const
{
    const VkResult result = vkBindBufferMemory(vkDevice, vkBuffer, vkDeviceMemory, 0);
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to bind buffer memory");
}

// ---------------------------------------------------------------------------------------------------------------------

void Buffer::destroy()
{
    if (vkBuffer) {
        vkDestroyBuffer(vkDevice, vkBuffer, nullptr);
        vkFreeMemory(vkDevice, vkDeviceMemory, nullptr);

        vkDevice = nullptr;
        vkBuffer = nullptr;
        vkDeviceMemory = nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

VkBuffer Buffer::getHandle() const
{
    return vkBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorBufferInfo& Buffer::getBufferInfo()
{
    return vkBufferInfo;
}

// ---------------------------------------------------------------------------------------------------------------------
