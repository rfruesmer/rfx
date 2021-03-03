#include "rfx/pch.h"
#include "VertexDiffuseEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

VertexDiffuseEffect::VertexDiffuseEffect(
    std::shared_ptr<GraphicsDevice> graphicsDevice,
    std::shared_ptr<Scene> scene)
        : graphicsDevice_(move(graphicsDevice)),
          scene_(move(scene)) {}

// ---------------------------------------------------------------------------------------------------------------------

VertexDiffuseEffect::~VertexDiffuseEffect()
{
    cleanupSwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::cleanupSwapChain()
{
    VkDevice device = graphicsDevice_->getLogicalDevice();
    vkDestroyDescriptorSetLayout(device, sceneDescSetLayout_, nullptr);
    vkDestroyDescriptorSetLayout(device, meshDescSetLayout_, nullptr);
    vkDestroyDescriptorSetLayout(device, materialDescSetLayout_, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::createUniformBuffers()
{
    createSceneDataBuffer();
    createMeshDataBuffers();
    createMaterialDataBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::createSceneDataBuffer()
{
    sceneDataBuffer_ = graphicsDevice_->createBuffer(
        sizeof(SceneData),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    graphicsDevice_->bind(sceneDataBuffer_);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::createMeshDataBuffers()
{
    for (const auto& mesh : scene_->getMeshes()) {
        shared_ptr<Buffer> meshDataBuffer = graphicsDevice_->createBuffer(
            sizeof(MeshData),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        graphicsDevice_->bind(meshDataBuffer);

        void* mappedMemory = nullptr;
        graphicsDevice_->map(meshDataBuffer, &mappedMemory);
        memcpy(mappedMemory, &mesh->getWorldTransform(), sizeof(mat4));
        graphicsDevice_->unmap(meshDataBuffer);

        meshDataBuffers_.push_back(meshDataBuffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::createMaterialDataBuffers()
{
    for (const auto& material : scene_->getMaterials()) {
        shared_ptr<Buffer> materialUniformBuffer = graphicsDevice_->createBuffer(
            sizeof(MaterialData),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        graphicsDevice_->bind(materialUniformBuffer);

        void* mappedMemory = nullptr;
        graphicsDevice_->map(materialUniformBuffer, &mappedMemory);
        memcpy(mappedMemory, &material->getBaseColorFactor(), sizeof(vec4));
        graphicsDevice_->unmap(materialUniformBuffer);

        materialDataBuffers_.push_back(materialUniformBuffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::setDescriptorPool(VkDescriptorPool descriptorPool)
{
    descriptorPool_ = descriptorPool;
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::createDescriptorSetLayouts()
{
    createSceneDescriptorSetLayout();
    createMaterialDescriptorSetLayout();
    createMeshDescriptorSetLayout();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::createSceneDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding sceneDescSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    };

    VkDescriptorSetLayoutCreateInfo sceneDescSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &sceneDescSetLayoutBinding
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice_->getLogicalDevice(),
        &sceneDescSetLayoutCreateInfo,
        nullptr,
        &sceneDescSetLayout_));
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::createMaterialDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding materialDescSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    };

    VkDescriptorSetLayoutCreateInfo materialDescSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &materialDescSetLayoutBinding
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice_->getLogicalDevice(),
        &materialDescSetLayoutCreateInfo,
        nullptr,
        &materialDescSetLayout_));
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::createMeshDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding meshDescSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    };

    VkDescriptorSetLayoutCreateInfo meshDescSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &meshDescSetLayoutBinding
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice_->getLogicalDevice(),
        &meshDescSetLayoutCreateInfo,
        nullptr,
        &meshDescSetLayout_));
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::createDescriptorSets()
{
    createSceneDescriptorSet();
    createMaterialDescriptorSets();
    createMeshDescriptorSets();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::createSceneDescriptorSet()
{
    VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool_,
        .descriptorSetCount = 1,
        .pSetLayouts = &sceneDescSetLayout_
    };

    ThrowIfFailed(vkAllocateDescriptorSets(
        graphicsDevice_->getLogicalDevice(),
        &allocInfo,
        &sceneDescSet_));

    VkDescriptorBufferInfo bufferInfo {
        .buffer = sceneDataBuffer_->getHandle(),
        .offset = 0,
        .range = VK_WHOLE_SIZE,
    };

    VkWriteDescriptorSet writeDescriptorSet {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = sceneDescSet_,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = &bufferInfo
    };

    vkUpdateDescriptorSets(
        graphicsDevice_->getLogicalDevice(),
        1,
        &writeDescriptorSet,
        0,
        nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::createMaterialDescriptorSets()
{
    materialDescSets_.resize(scene_->getMaterialCount());

    for (uint32_t i = 0, count = scene_->getMaterialCount(); i < count; ++i) {

        VkDescriptorSetAllocateInfo allocInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = descriptorPool_,
            .descriptorSetCount = 1,
            .pSetLayouts = &materialDescSetLayout_
        };

        ThrowIfFailed(vkAllocateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            &allocInfo,
            &materialDescSets_[i]));

        VkDescriptorBufferInfo bufferInfo {
            .buffer = materialDataBuffers_[i]->getHandle(),
            .offset = 0,
            .range = VK_WHOLE_SIZE,
        };

        VkWriteDescriptorSet writeDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = materialDescSets_[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &bufferInfo
        };

        vkUpdateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            1,
            &writeDescriptorSet,
            0,
            nullptr);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::createMeshDescriptorSets()
{
    meshDescSets_.resize(scene_->getMeshCount());

    for (uint32_t i = 0, count = scene_->getMeshCount(); i < count; ++i) {

        VkDescriptorSetAllocateInfo allocInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = descriptorPool_,
            .descriptorSetCount = 1,
            .pSetLayouts = &meshDescSetLayout_
        };

        ThrowIfFailed(vkAllocateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            &allocInfo,
            &meshDescSets_[i]));

        VkDescriptorBufferInfo bufferInfo {
            .buffer = meshDataBuffers_[i]->getHandle(),
            .offset = 0,
            .range = VK_WHOLE_SIZE,
        };

        VkWriteDescriptorSet writeDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = meshDescSets_[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &bufferInfo
        };

        vkUpdateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            1,
            &writeDescriptorSet,
            0,
            nullptr);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

vector<VkDescriptorSetLayout> VertexDiffuseEffect::getDescriptorSetLayouts()
{
    return {
        sceneDescSetLayout_,
        materialDescSetLayout_,
        meshDescSetLayout_
    };
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorSet VertexDiffuseEffect::getSceneDescSet() const
{
    return sceneDescSet_;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<VkDescriptorSet>& VertexDiffuseEffect::getMaterialDescSets() const
{
    return materialDescSets_;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<VkDescriptorSet>& VertexDiffuseEffect::getMeshDescSets() const
{
    return meshDescSets_;
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::setProjection(const mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;
    sceneData_.lightPos = viewMatrix * vec4(lightPosition_, 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::setLightPosition(const vec3& lightPosition)
{
    lightPosition_ = lightPosition;
    sceneData_.lightPos = sceneData_.viewMatrix * vec4(lightPosition, 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::setLightDiffuseColor(const vec3& diffuseColor)
{
    sceneData_.Ld = vec4(diffuseColor, 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseEffect::updateSceneDataMemory()
{
    void* mappedMemory = nullptr;
    graphicsDevice_->map(sceneDataBuffer_, &mappedMemory);
    memcpy(mappedMemory, &sceneData_, sizeof(SceneData));
    graphicsDevice_->unmap(sceneDataBuffer_);
}

// ---------------------------------------------------------------------------------------------------------------------

