#include "rfx/pch.h"
#include "TexturedMultiLightEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

TexturedMultiLightEffect::TexturedMultiLightEffect(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    const shared_ptr<Scene>& scene)
        : TestEffect(graphicsDevice, scene) {}

// ---------------------------------------------------------------------------------------------------------------------

string TexturedMultiLightEffect::getVertexShaderFileName() const
{
    return "textured_multilight.vert";
}

// ---------------------------------------------------------------------------------------------------------------------

string TexturedMultiLightEffect::getFragmentShaderFileName() const
{
    return "textured_multilight.frag";
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightEffect::createDescriptorPool()
{
    const SwapChainDesc& swapChainDesc = graphicsDevice_->getSwapChain()->getDesc();

    const uint32_t meshCount = scene_->getMeshCount();
    const uint32_t materialCount = scene_->getMaterialCount();

    vector<VkDescriptorPoolSize> poolSizes {
        {
            // 1 for scene, 1 per material and 1 per mesh
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1 + materialCount + meshCount
        },
        {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = materialCount
        }
    };

    VkDescriptorPoolCreateInfo poolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = 1 + materialCount + meshCount, // TODO
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data()
    };

    ThrowIfFailed(vkCreateDescriptorPool(
        graphicsDevice_->getLogicalDevice(),
        &poolCreateInfo,
        nullptr,
        &descriptorPool_));
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightEffect::createDescriptorSetLayouts()
{
    vector<VkDescriptorSetLayoutBinding> descSetLayoutBindings {
        {   // scene
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
        },
        {   // mesh
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
        },
        {   // material
            .binding = 2,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
        },
        {   // texture
            .binding = 3,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        }
    };

    VkDescriptorSetLayoutCreateInfo descSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast<uint32_t>(descSetLayoutBindings.size()),
        .pBindings = descSetLayoutBindings.data()
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice_->getLogicalDevice(),
        &descSetLayoutCreateInfo,
        nullptr,
        &descSetLayout_));
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightEffect::createDescriptorSets()
{
    descSets_.resize(scene_->getMeshCount());

    for (uint32_t i = 0, count = scene_->getMeshCount(); i < count; ++i) {

        VkDescriptorSetAllocateInfo allocInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = descriptorPool_,
            .descriptorSetCount = 1,
            .pSetLayouts = &descSetLayout_
        };

        ThrowIfFailed(vkAllocateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            &allocInfo,
            &descSets_[i]));

        VkDescriptorBufferInfo sceneBufferInfo {
            .buffer = sceneDataBuffer_->getHandle(),
            .offset = 0,
            .range = VK_WHOLE_SIZE,
        };

        VkDescriptorBufferInfo meshBufferInfo {
            .buffer = meshDataBuffers_[i]->getHandle(),
            .offset = 0,
            .range = VK_WHOLE_SIZE,
        };

        VkDescriptorBufferInfo materialBufferInfo {
            .buffer = meshDataBuffers_[i]->getHandle(),
            .offset = 0,
            .range = VK_WHOLE_SIZE,
        };

        const shared_ptr<Texture2D>& baseColorTexture =
            scene_->getMaterial(i)->getBaseColorTexture();
        VkDescriptorImageInfo textureImageInfo {
            .sampler = baseColorTexture->getSampler(),
            .imageView = baseColorTexture->getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        vector<VkWriteDescriptorSet> writeDescriptorSets = {
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descSets_[i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &sceneBufferInfo
            },
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descSets_[i],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &meshBufferInfo
            },
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descSets_[i],
                .dstBinding = 2,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &materialBufferInfo
            },
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descSets_[i],
                .dstBinding = 3,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &textureImageInfo
            }
        };

        vkUpdateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            writeDescriptorSets.size(),
            writeDescriptorSets.data(),
            0,
            nullptr);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightEffect::setProjectionMatrix(const glm::mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightEffect::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (lights_[i] != nullptr) {
            sceneData_.lights[i].position = viewMatrix * vec4(lights_[i]->getPosition(), 1.0f);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightEffect::setLight(int index, const shared_ptr<PointLight>& light)
{
    lights_[index] = light;
    if (light == nullptr) {
        sceneData_.lights[index].enabled = false;
        return;
    }

    auto& sceneDataLight = sceneData_.lights[index];
    sceneDataLight.type = light->getType();
    sceneDataLight.enabled = true;
    sceneDataLight.position = sceneData_.viewMatrix * vec4(light->getPosition(), 1.0f);
    sceneDataLight.color = vec4(light->getColor(), 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightEffect::setLight(int index, const shared_ptr<SpotLight>& light)
{
    setLight(index, static_pointer_cast<PointLight>(light));
    if (light == nullptr) {
        return;
    }

    auto& sceneDataLight = sceneData_.lights[index];
    sceneDataLight.direction = light->getDirection();
    sceneDataLight.exponent = light->getExponent();
    sceneDataLight.cutoff = light->getCutoff();
}

// ---------------------------------------------------------------------------------------------------------------------

size_t TexturedMultiLightEffect::getSceneDataSize() const
{
    return sizeof(SceneData);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightEffect::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<VkDescriptorSet>& TexturedMultiLightEffect::getDescriptorSets()
{
    return descSets_;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<VkDescriptorSetLayout> TexturedMultiLightEffect::getDescriptorSetLayouts()
{
    return {
        descSetLayout_
    };
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightEffect::cleanupSwapChain()
{
    VkDevice device = graphicsDevice_->getLogicalDevice();
    vkDestroyDescriptorSetLayout(device, descSetLayout_, nullptr);

    TestEffect::cleanupSwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------
