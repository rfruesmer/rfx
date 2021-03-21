#include "rfx/pch.h"
#include "TestEffect.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

TestEffect::TestEffect(
    shared_ptr<GraphicsDevice> graphicsDevice,
    shared_ptr<Model> scene)
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
            .specular = material->getSpecularFactor(),
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
    const uint32_t meshCount = scene_->getGeometryNodeCount();
    const uint32_t materialCount = scene_->getMaterialCount();
    const uint32_t uniformBufferCount = 1 + meshCount + materialCount;

    uint32_t imageSamplerCount = getVertexFormat().containsTexCoords() ? 1 : 0;
    imageSamplerCount += getVertexFormat().containsTangents() ? 1 : 0;
    imageSamplerCount += scene_->getMaterial(0)->getMetallicRoughnessTexture() != nullptr ? 1 : 0; // TODO: extract imageSamplerCount to argument
    imageSamplerCount += scene_->getMaterial(0)->getOcclusionTexture() != nullptr ? 1 : 0; // TODO: extract imageSamplerCount to argument
    imageSamplerCount += scene_->getMaterial(0)->getEmissiveTexture() != nullptr ? 1 : 0; // TODO: extract imageSamplerCount to argument

    vector<VkDescriptorPoolSize> descriptorPoolSizes = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uniformBufferCount}
    };

    if (imageSamplerCount > 0) {
        descriptorPoolSizes.push_back({ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, imageSamplerCount });
    }

    descriptorPool_ = createDescriptorPool(descriptorPoolSizes, uniformBufferCount + imageSamplerCount);
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
    createMeshDescriptorSetLayout();
    createMaterialDescriptorSetLayout();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createSceneDescriptorSetLayout()
{
    const VkDescriptorSetLayoutBinding sceneDescSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
    };

    const VkDescriptorSetLayoutCreateInfo sceneDescSetLayoutCreateInfo {
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

void TestEffect::createMeshDescriptorSetLayout()
{
    const VkDescriptorSetLayoutBinding meshDescSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
    };

    const VkDescriptorSetLayoutCreateInfo meshDescSetLayoutCreateInfo {
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

void TestEffect::createMaterialDescriptorSetLayout()
{
    vector<VkDescriptorSetLayoutBinding> materialDescSetLayoutBindings;
    uint32_t binding = 0;


    materialDescSetLayoutBindings.push_back({
        .binding = binding++,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
    });

    if (getVertexFormat().containsTexCoords()) {
        materialDescSetLayoutBindings.push_back({
            .binding = binding++,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        });
    }

    if (getVertexFormat().containsTangents()) {
        materialDescSetLayoutBindings.push_back({
            .binding = binding++,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        });
    }

    if (scene_->getMaterial(0)->getMetallicRoughnessTexture() != nullptr) { // TODO: should be defined on a per-effect basis
        materialDescSetLayoutBindings.push_back({
            .binding = binding++,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        });
    }

    if (scene_->getMaterial(0)->getOcclusionTexture() != nullptr) { // TODO: should be defined on a per-effect basis
        materialDescSetLayoutBindings.push_back({
            .binding = binding++,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        });
    }

    if (scene_->getMaterial(0)->getEmissiveTexture() != nullptr) { // TODO: should be defined on a per-effect basis
        materialDescSetLayoutBindings.push_back({
            .binding = binding++,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        });
    }

    const VkDescriptorSetLayoutCreateInfo materialDescSetLayoutCreateInfo {
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

void TestEffect::createDescriptorSets()
{
    createSceneDescriptorSet();
    createMeshDescriptorSets();
    createMaterialDescriptorSets();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createSceneDescriptorSet()
{
    const VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool_,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayouts_[DescriptorType::SCENE]
    };

    ThrowIfFailed(vkAllocateDescriptorSets(
        graphicsDevice_->getLogicalDevice(),
        &allocInfo,
        &sceneDescriptorSet_));

    const VkWriteDescriptorSet writeDescriptorSet =
        buildWriteDescriptorSet(sceneDescriptorSet_, 0, &sceneDataBuffer_->getDescriptorBufferInfo());

    vkUpdateDescriptorSets(
        graphicsDevice_->getLogicalDevice(),
        1,
        &writeDescriptorSet,
        0,
        nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createMeshDescriptorSets()
{
    meshDescriptorSets_.resize(scene_->getGeometryNodeCount());

    const VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool_,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayouts_[DescriptorType::MESH]
    };

    for (uint32_t i = 0, count = scene_->getGeometryNodeCount(); i < count; ++i) {

        ThrowIfFailed(vkAllocateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            &allocInfo,
            &meshDescriptorSets_[i]));

        const VkWriteDescriptorSet writeDescriptorSet =
            buildWriteDescriptorSet(
                meshDescriptorSets_[i],
                0,
                &meshDataBuffers_[i]->getDescriptorBufferInfo());

        vkUpdateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            1,
            &writeDescriptorSet,
            0,
            nullptr);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createMaterialDescriptorSets()
{
    materialDescriptorSets_.resize(scene_->getMaterialCount());

    const VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool_,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayouts_[DescriptorType::MATERIAL]
    };

    for (uint32_t i = 0, count = scene_->getMaterialCount(); i < count; ++i) {

        const shared_ptr<Material>& material = scene_->getMaterial(i);
        vector<VkWriteDescriptorSet> writeDescriptorSets;
        uint32_t binding = 0;

        ThrowIfFailed(vkAllocateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            &allocInfo,
            &materialDescriptorSets_[i]));

        writeDescriptorSets.push_back(
            buildWriteDescriptorSet(
                materialDescriptorSets_[i],
                binding++,
                &materialDataBuffers_[i]->getDescriptorBufferInfo()));

        if (const auto& baseColorTexture = material->getBaseColorTexture()) {
            writeDescriptorSets.push_back(
                buildWriteDescriptorSet(
                    materialDescriptorSets_[i],
                    binding++,
                    &baseColorTexture->getDescriptorImageInfo()));
        }

        if (const auto& normalTexture = material->getNormalTexture()) {
            writeDescriptorSets.push_back(
                buildWriteDescriptorSet(
                    materialDescriptorSets_[i],
                    binding++,
                    &normalTexture->getDescriptorImageInfo()));
        }

        if (const auto& metallicRoughnessTexture = material->getMetallicRoughnessTexture()) {
            writeDescriptorSets.push_back(
                buildWriteDescriptorSet(
                    materialDescriptorSets_[i],
                    binding++,
                    &metallicRoughnessTexture->getDescriptorImageInfo()));
        }

        if (const auto& occlusionTexture = material->getOcclusionTexture()) {
            writeDescriptorSets.push_back(
                buildWriteDescriptorSet(
                    materialDescriptorSets_[i],
                    binding++,
                    &occlusionTexture->getDescriptorImageInfo()));
        }

        if (const auto& emissiveTexture = material->getEmissiveTexture()) {
            writeDescriptorSets.push_back(
                buildWriteDescriptorSet(
                    materialDescriptorSets_[i],
                    binding++,
                    &emissiveTexture->getDescriptorImageInfo()));
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

VkWriteDescriptorSet TestEffect::buildWriteDescriptorSet(
    VkDescriptorSet descriptorSet,
    uint32_t binding,
    const VkDescriptorImageInfo* descriptorImageInfo)
{
    return {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = descriptorImageInfo
    };
}

// ---------------------------------------------------------------------------------------------------------------------

VkWriteDescriptorSet TestEffect::buildWriteDescriptorSet(
    VkDescriptorSet descriptorSet,
    uint32_t binding,
    const VkDescriptorBufferInfo* descriptorBufferInfo)
{
    return {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = descriptorBufferInfo
    };
}

// ---------------------------------------------------------------------------------------------------------------------

VertexFormat TestEffect::getVertexFormat() const
{
    return scene_->getMaterial(0)->getVertexFormat(); // TODO: support for multiple/different vertex formats
}

// ---------------------------------------------------------------------------------------------------------------------

vector<VkDescriptorSetLayout> TestEffect::getDescriptorSetLayouts() const
{
    return descriptorSetLayouts_;
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorSet TestEffect::getSceneDescriptorSet() const
{
    return sceneDescriptorSet_;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<VkDescriptorSet>& TestEffect::getMeshDescriptorSets() const
{
    return meshDescriptorSets_;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<VkDescriptorSet>& TestEffect::getMaterialDescriptorSets() const
{
    return materialDescriptorSets_;
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::cleanupSwapChain()
{
    VkDevice device = graphicsDevice_->getLogicalDevice();
    for (int i = 0; i < DescriptorType::QUANTITY; ++i) {
        vkDestroyDescriptorSetLayout(device, descriptorSetLayouts_[i], nullptr);
    }
    vkDestroyDescriptorPool(device, descriptorPool_, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

