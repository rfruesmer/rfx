#include "rfx/pch.h"
#include "TestEffect.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

TestEffect::TestEffect(
    shared_ptr<GraphicsDevice> graphicsDevice,
    shared_ptr<Scene> scene)
    : graphicsDevice_(move(graphicsDevice)),
      scene_(move(scene)) {}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createUniformBuffers()
{
    createSceneDataBuffer();
    createMeshDataBuffers();
    createMaterialDataBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createSceneDataBuffer()
{
    sceneDataBuffer_ = graphicsDevice_->createBuffer(
        getSceneDataSize(),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    graphicsDevice_->bind(sceneDataBuffer_);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createMeshDataBuffers()
{
    for (const auto& node : scene_->getGeometryNodes()) {
        shared_ptr<Buffer> meshDataBuffer = graphicsDevice_->createBuffer(
            sizeof(MeshData),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        graphicsDevice_->bind(meshDataBuffer);
        meshDataBuffer->load(sizeof(mat4), &node->getWorldTransform());
        meshDataBuffers_.push_back(meshDataBuffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createMaterialDataBuffers()
{
    for (const auto& material : scene_->getMaterials()) {
        shared_ptr<Buffer> materialUniformBuffer = graphicsDevice_->createBuffer(
            sizeof(MaterialData),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        MaterialData materialData {
            .baseColor = material->getBaseColorFactor(),
            .shininess = material->getShininess()
        };

        graphicsDevice_->bind(materialUniformBuffer);
        materialUniformBuffer->load(sizeof(MaterialData), &materialData);
        materialDataBuffers_.push_back(materialUniformBuffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createDescriptorPools()
{
    const uint32_t geometryNodesCount = scene_->getGeometryNodeCount();
    const uint32_t materialCount = scene_->getMaterialCount();

    descriptorPools_[DescriptorType::SCENE] = createDescriptorPool(
        {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}}, 1);
    descriptorPools_[DescriptorType::MESH] = createDescriptorPool(
        {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, geometryNodesCount}}, geometryNodesCount);

    vector<VkDescriptorPoolSize> materialPoolSizes = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, materialCount}
    };
    if (getVertexFormat().containsTexCoords()) {
        materialPoolSizes.push_back({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, materialCount});
    }
    if (getVertexFormat().containsTangents()) {
        materialPoolSizes.push_back({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, materialCount});
    }

    descriptorPools_[DescriptorType::MATERIAL] = createDescriptorPool(
        materialPoolSizes, materialCount);
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorPool TestEffect::createDescriptorPool(
    const vector<VkDescriptorPoolSize>& poolSizes,
    uint32_t maxSets)
{
    VkDescriptorPoolCreateInfo poolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = maxSets,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data()
    };

    VkDescriptorPool descriptorPool;
    ThrowIfFailed(vkCreateDescriptorPool(
        graphicsDevice_->getLogicalDevice(),
        &poolCreateInfo,
        nullptr,
        &descriptorPool));

    return descriptorPool;
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createDescriptorSetLayouts()
{
    createSceneDescriptorSetLayout();
    createMaterialDescriptorSetLayout();
    createMeshDescriptorSetLayout();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createSceneDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding sceneDescSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
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
        &descriptorSetLayouts_[DescriptorType::SCENE]));
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createMaterialDescriptorSetLayout()
{
    vector<VkDescriptorSetLayoutBinding> materialDescSetLayoutBindings {
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
        }
    };

    if (getVertexFormat().containsTexCoords()) {
        materialDescSetLayoutBindings.push_back({
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        });
    }

    if (getVertexFormat().containsTangents()) {
        materialDescSetLayoutBindings.push_back({
            .binding = 2,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        });
    }

    VkDescriptorSetLayoutCreateInfo materialDescSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast<uint32_t>(materialDescSetLayoutBindings.size()),
        .pBindings = materialDescSetLayoutBindings.data()
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice_->getLogicalDevice(),
        &materialDescSetLayoutCreateInfo,
        nullptr,
        &descriptorSetLayouts_[DescriptorType::MATERIAL]));
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createMeshDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding meshDescSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
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
        &descriptorSetLayouts_[DescriptorType::MESH]));
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createDescriptorSets()
{
    createSceneDescriptorSet();
    createMaterialDescriptorSets();
    createMeshDescriptorSets();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createSceneDescriptorSet()
{
    VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPools_[DescriptorType::SCENE],
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayouts_[DescriptorType::SCENE]
    };

    ThrowIfFailed(vkAllocateDescriptorSets(
        graphicsDevice_->getLogicalDevice(),
        &allocInfo,
        &sceneDescriptorSet_));

    VkDescriptorBufferInfo bufferInfo {
        .buffer = sceneDataBuffer_->getHandle(),
        .offset = 0,
        .range = VK_WHOLE_SIZE,
    };

    VkWriteDescriptorSet writeDescriptorSet {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = sceneDescriptorSet_,
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

void TestEffect::createMaterialDescriptorSets()
{
    materialDescriptorSets_.resize(scene_->getMaterialCount());

    for (uint32_t i = 0, count = scene_->getMaterialCount(); i < count; ++i) {

        VkDescriptorSetAllocateInfo allocInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = descriptorPools_[DescriptorType::MATERIAL],
            .descriptorSetCount = 1,
            .pSetLayouts = &descriptorSetLayouts_[DescriptorType::MATERIAL]
        };

        ThrowIfFailed(vkAllocateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            &allocInfo,
            &materialDescriptorSets_[i]));

        VkDescriptorBufferInfo bufferInfo {
            .buffer = materialDataBuffers_[i]->getHandle(),
            .offset = 0,
            .range = VK_WHOLE_SIZE,
        };

        vector<VkWriteDescriptorSet> writeDescriptorSets {
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = materialDescriptorSets_[i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &bufferInfo
            }
        };

        if (getVertexFormat().containsTexCoords()) {

            const shared_ptr<Texture2D>& baseColorTexture =
                scene_->getMaterial(i)->getBaseColorTexture();

            VkDescriptorImageInfo textureImageInfo = {
                .sampler = baseColorTexture->getSampler(),
                .imageView = baseColorTexture->getImageView(),
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };

            writeDescriptorSets.push_back({
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = materialDescriptorSets_[i],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &textureImageInfo
            });
        }

        if (getVertexFormat().containsTangents()) {

            const shared_ptr<Texture2D>& normalTexture =
                scene_->getMaterial(i)->getNormalTexture();

            VkDescriptorImageInfo textureImageInfo = {
                .sampler = normalTexture->getSampler(),
                .imageView = normalTexture->getImageView(),
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };

            writeDescriptorSets.push_back({
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = materialDescriptorSets_[i],
                .dstBinding = 2,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &textureImageInfo
            });
        }

        vkUpdateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            writeDescriptorSets.size(),
            writeDescriptorSets.data(),
            0,
            nullptr);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createMeshDescriptorSets()
{
    meshDescriptorSets_.resize(scene_->getGeometryNodeCount());

    for (uint32_t i = 0, count = scene_->getGeometryNodeCount(); i < count; ++i) {

        VkDescriptorSetAllocateInfo allocInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = descriptorPools_[DescriptorType::MESH],
            .descriptorSetCount = 1,
            .pSetLayouts = &descriptorSetLayouts_[DescriptorType::MESH]
        };

        ThrowIfFailed(vkAllocateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            &allocInfo,
            &meshDescriptorSets_[i]));

        VkDescriptorBufferInfo bufferInfo {
            .buffer = meshDataBuffers_[i]->getHandle(),
            .offset = 0,
            .range = VK_WHOLE_SIZE,
        };

        VkWriteDescriptorSet writeDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = meshDescriptorSets_[i],
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

vector<VkDescriptorSetLayout> TestEffect::getDescriptorSetLayouts()
{
    return descriptorSetLayouts_;
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorSet TestEffect::getSceneDescriptorSet() const
{
    return sceneDescriptorSet_;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<VkDescriptorSet>& TestEffect::getMaterialDescriptorSets() const
{
    return materialDescriptorSets_;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<VkDescriptorSet>& TestEffect::getMeshDescriptorSets() const
{
    return meshDescriptorSets_;
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::cleanupSwapChain()
{
    VkDevice device = graphicsDevice_->getLogicalDevice();
    for (int i = 0; i < DescriptorType::QUANTITY; ++i) {
        vkDestroyDescriptorSetLayout(device, descriptorSetLayouts_[i], nullptr);
        vkDestroyDescriptorPool(device, descriptorPools_[i], nullptr);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

