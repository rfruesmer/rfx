#include "rfx/pch.h"
#include "rfx/application/DevTools.h"

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

DevTools::DevTools(
    const shared_ptr<Window>& window,
    const unique_ptr<GraphicsContext>& graphicsContext,
    shared_ptr<GraphicsDevice> graphicsDevice)
        : graphicsDevice_(move(graphicsDevice))
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window->getGlfwWindow(), true);

    VkDescriptorPoolSize poolSizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

    VkDescriptorPoolCreateInfo poolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = 1000 * IM_ARRAYSIZE(poolSizes),
        .poolSizeCount = (uint32_t)IM_ARRAYSIZE(poolSizes),
        .pPoolSizes = poolSizes
    };

    ThrowIfFailed(vkCreateDescriptorPool(
        graphicsDevice_->getLogicalDevice(),
        &poolCreateInfo,
        nullptr,
        &descriptorPool));

    const unique_ptr<SwapChain>& swapChain = graphicsDevice_->getSwapChain();
    const SwapChainDesc& swapChainDesc = swapChain->getDesc();

    ImGui_ImplVulkan_InitInfo init_info {
        .Instance = graphicsContext->getInstance(),
        .PhysicalDevice = graphicsDevice_->getPhysicalDevice(),
        .Device = graphicsDevice_->getLogicalDevice(),
        .QueueFamily = graphicsDevice_->getGraphicsQueue()->getFamilyIndex(),
        .Queue = graphicsDevice_->getGraphicsQueue()->getHandle(),
        .PipelineCache = VK_NULL_HANDLE,
        .DescriptorPool = descriptorPool,
        .MinImageCount = swapChainDesc.bufferCount,
        .ImageCount = swapChainDesc.bufferCount,
        .Allocator = nullptr,
        .CheckVkResultFn = checkResult,
    };

    VkAttachmentDescription attachment {
        .format = swapChainDesc.format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD, // wd->ClearEnable ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentReference colorAttachment {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpass {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachment,
    };

    VkSubpassDependency subpassDependency {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
    };

    VkRenderPassCreateInfo renderPassCreateInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &attachment,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &subpassDependency
    };
    
    ThrowIfFailed(vkCreateRenderPass(
        graphicsDevice_->getLogicalDevice(),
        &renderPassCreateInfo,
        nullptr,
        &renderPass));

    ImGui_ImplVulkan_Init(&init_info, renderPass);

    const shared_ptr<CommandBuffer>& commandBuffer =
        graphicsDevice_->createCommandBuffer(graphicsDevice_->getGraphicsCommandPool());
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer->getHandle());
    commandBuffer->end();
    graphicsDevice_->flush(commandBuffer);
    ImGui_ImplVulkan_DestroyFontUploadObjects();

    createCommandPool(&commandPool_, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    commandBuffers_.resize(swapChainDesc.bufferCount);
    createCommandBuffers(commandBuffers_.size(), commandPool_, commandBuffers_.data());

    VkImageView imageView;

    VkFramebufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.renderPass = renderPass;
    info.attachmentCount = 1;
    info.pAttachments = &imageView;
    info.width = swapChainDesc.extent.width;
    info.height = swapChainDesc.extent.height;
    info.layers = 1;

    frameBuffers.resize(swapChainDesc.bufferCount);

    for (uint32_t i = 0; i < swapChainDesc.bufferCount; i++)
    {
        imageView = swapChain->getImageViews()[i];

        ThrowIfFailed(vkCreateFramebuffer(
            graphicsDevice_->getLogicalDevice(), 
            &info, 
            nullptr, 
            &frameBuffers[i]));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DevTools::checkResult(VkResult result)
{
    ThrowIfFailed(result);
}

// ---------------------------------------------------------------------------------------------------------------------

void DevTools::createCommandPool(VkCommandPool* commandPool, VkCommandPoolCreateFlags createFlags)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = createFlags,
        .queueFamilyIndex = graphicsDevice_->getGraphicsQueue()->getFamilyIndex()
    };

    ThrowIfFailed(vkCreateCommandPool(
        graphicsDevice_->getLogicalDevice(),
        &commandPoolCreateInfo,
        nullptr,
        commandPool));
}

// ---------------------------------------------------------------------------------------------------------------------

void DevTools::createCommandBuffers(
    uint32_t count,
    VkCommandPool commandPool,
    VkCommandBuffer* outCommandBuffers)
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = count
    };

    ThrowIfFailed(vkAllocateCommandBuffers(
        graphicsDevice_->getLogicalDevice(),
        &commandBufferAllocateInfo,
        outCommandBuffers));
}

// ---------------------------------------------------------------------------------------------------------------------

void DevTools::beginDraw(uint32_t frameIndex, uint32_t lastFPS)
{
    currentFrameIndex = frameIndex;

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    ImGui::Begin("DevTools", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%s", graphicsDevice_->getDesc().properties.deviceName);
    ImGui::Text("%.2f ms/frame (%.1d fps)", (1000.0f / lastFPS), lastFPS);
    ImGui::NewLine();
}

// ---------------------------------------------------------------------------------------------------------------------

void DevTools::endDraw()
{
    ImGui::End();
    ImGui::Render();

    VkCommandBufferBeginInfo commandBufferBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    ThrowIfFailed(vkBeginCommandBuffer(
        commandBuffers_[currentFrameIndex],
        &commandBufferBeginInfo));

    const SwapChainDesc& swapChainDesc = graphicsDevice_->getSwapChain()->getDesc();
    VkClearValue clearValue {
        .color {0.45f, 0.55f, 0.60f, 1.00f }
    };

    VkRenderPassBeginInfo renderPassBeginInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = renderPass,
        .framebuffer = frameBuffers[currentFrameIndex],
        .renderArea {
            .extent { swapChainDesc.extent.width, swapChainDesc.extent.height }
        },
        .clearValueCount = 1,
        .pClearValues = &clearValue,
    };

    vkCmdBeginRenderPass(commandBuffers_[currentFrameIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    ImDrawData* draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffers_[currentFrameIndex]);
    vkCmdEndRenderPass(commandBuffers_[currentFrameIndex]);
    ThrowIfFailed(vkEndCommandBuffer(
        commandBuffers_[currentFrameIndex]));
}

// ---------------------------------------------------------------------------------------------------------------------

VkCommandBuffer DevTools::getCommandBuffer(uint32_t frameIndex) const
{
    return commandBuffers_[frameIndex];
}

// ---------------------------------------------------------------------------------------------------------------------

DevTools::~DevTools()
{
    VkDevice device = graphicsDevice_->getLogicalDevice();

    vkFreeCommandBuffers(device, commandPool_, static_cast<uint32_t>(commandBuffers_.size()), commandBuffers_.data());
    vkDestroyCommandPool(device, commandPool_, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    for (const auto& frameBuffer : frameBuffers) {
        vkDestroyFramebuffer(device, frameBuffer, nullptr);
    }

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

bool DevTools::sliderFloat(const char* caption, float* value, float min, float max)
{
    return ImGui::SliderFloat(caption, value, min, max);
}

// ---------------------------------------------------------------------------------------------------------------------

bool DevTools::checkBox(const char* caption, bool* value)
{
    return ImGui::Checkbox(caption, value);
}

// ---------------------------------------------------------------------------------------------------------------------
