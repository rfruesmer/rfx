#include "rfx/pch.h"
#include "rfx/graphics/CommandPool.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

CommandPool::CommandPool(VkDevice vkDevice, 
    VkCommandPool vkCommandPool, 
    const VulkanDeviceFunctionPtrs& vk)
        : vkDevice(vkDevice),
          vkCommandPool(vkCommandPool),
          vk(vk)
{
    vkAllocateCommandBuffers = vk.vkAllocateCommandBuffers;    
    vkFreeCommandBuffers = vk.vkFreeCommandBuffers;
    vkResetCommandBuffer = vk.vkResetCommandBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------

CommandPool::~CommandPool()
{
    int i = 42;
}

// ---------------------------------------------------------------------------------------------------------------------

VkCommandPool CommandPool::getHandle() const
{
    return vkCommandPool;
}

// ---------------------------------------------------------------------------------------------------------------------

bool CommandPool::isValid() const
{
    return vkCommandPool != nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandPool::invalidate()
{
    vkCommandPool = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandPool::clear()
{
    unordered_set<shared_ptr<CommandBuffer>> copyOfCommandBuffers(commandBuffers);

    for (const auto& it : copyOfCommandBuffers) {
        freeCommandBuffer(it);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<CommandBuffer> CommandPool::createCommandBuffer()
{
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = vkCommandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer vkCommandBuffer = nullptr;

    const VkResult result = vkAllocateCommandBuffers(vkDevice, &allocateInfo, &vkCommandBuffer);
    RFX_CHECK_STATE(result == VK_SUCCESS && vkCommandBuffer != nullptr,
        "Failed to allocate command buffers");

    shared_ptr<CommandBuffer> commandBuffer = make_shared<CommandBuffer>(vkCommandBuffer, vk);
    commandBuffers.insert(commandBuffer);

    return commandBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandPool::freeCommandBuffer(const shared_ptr<CommandBuffer>& commandBuffer)
{
    freeCommandBuffers({commandBuffer});
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandPool::freeCommandBuffers(const vector<shared_ptr<CommandBuffer>>& commandBuffers)
{
    const size_t count = commandBuffers.size();
    vector<VkCommandBuffer> vkCommandBuffers(count);

    for (size_t i = 0; i < count; ++i) {
        const auto& currentBuffer = commandBuffers[i];
        RFX_CHECK_STATE(currentBuffer->isValid(), "Invalid command buffer detected");
        vkCommandBuffers[i] = currentBuffer->getHandle();
        currentBuffer->invalidate();
        this->commandBuffers.erase(currentBuffer);
    }

    vkFreeCommandBuffers(vkDevice, vkCommandPool, 1, vkCommandBuffers.data());
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandPool::freeAllCommandBuffers()
{
    const vector<shared_ptr<CommandBuffer>> allCommandBuffers(
        this->commandBuffers.begin(), this->commandBuffers.end());
    freeCommandBuffers(allCommandBuffers);
}

// ---------------------------------------------------------------------------------------------------------------------

