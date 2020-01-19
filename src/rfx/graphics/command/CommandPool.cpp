#include "rfx/pch.h"
#include "rfx/graphics/command/CommandPool.h"

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

shared_ptr<CommandBuffer> CommandPool::allocateCommandBuffer()
{
    vector<shared_ptr<CommandBuffer>> commandBuffers = allocateCommandBuffers(1);

    return commandBuffers[0];
}

// ---------------------------------------------------------------------------------------------------------------------

vector<shared_ptr<CommandBuffer>> CommandPool::allocateCommandBuffers(size_t count)
{
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = vkCommandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = static_cast<uint32_t>(count);

    vector<VkCommandBuffer> vkCommandBuffers(count);

    const VkResult result = vkAllocateCommandBuffers(vkDevice, &allocateInfo, vkCommandBuffers.data());
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to allocate command buffers");

    vector<shared_ptr<CommandBuffer>> commandBuffers(count);
    for (size_t i = 0; i < count; ++i) {
        const shared_ptr<CommandBuffer> commandBuffer = make_shared<CommandBuffer>(vkCommandBuffers[i], vk);
        commandBuffers[i] = commandBuffer;
        this->commandBuffers.insert(commandBuffer);
    }

    return commandBuffers;
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

