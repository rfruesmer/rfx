#include "rfx/pch.h"
#include "rfx/scene/MaterialShaderFactory.h"


using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

MaterialShaderFactory::MaterialShaderFactory(
    GraphicsDevicePtr graphicsDevice,
    VkDescriptorPool descriptorPool,
    std::filesystem::path shadersDirectory,
    std::string defaultShaderId)
    : graphicsDevice(move(graphicsDevice)),
      descriptorPool(descriptorPool),
      shadersDirectory(move(shadersDirectory)),
      defaultShaderId(move(defaultShaderId)) {}

// ---------------------------------------------------------------------------------------------------------------------

void MaterialShaderFactory::addAllocator(
    const string& shaderId,
    const function<MaterialShaderPtr()>& allocator)
{
    RFX_CHECK_STATE(!allocatorMap.contains(shaderId), "");

    allocatorMap[shaderId] = allocator;
}

// ---------------------------------------------------------------------------------------------------------------------

MaterialShaderPtr MaterialShaderFactory::createShaderFor(const MaterialPtr& material)
{
    MaterialShaderPtr shader = getCachedShaderFor(material);
    if (shader == nullptr) {
        shader = createShader(material);
        addToCache(shader, material);
    }

    return shader;
}

// ---------------------------------------------------------------------------------------------------------------------

MaterialShaderPtr MaterialShaderFactory::getCachedShaderFor(const MaterialPtr& material)
{
    const size_t shaderHash = hash(material);

    return shaderCache.get(shaderHash);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t MaterialShaderFactory::hash(const MaterialPtr& material)
{
    string shaderId = material->getShaderId();
    shaderId = shaderId.empty() ? defaultShaderId : shaderId;

    const auto it = allocatorMap.find(shaderId);
    RFX_CHECK_ARGUMENT(it != allocatorMap.end());

    const auto allocator = it->second;
    MaterialShaderPtr shader = allocator();

    return hash(shader, material);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t MaterialShaderFactory::hash(const MaterialShaderPtr& shader, const MaterialPtr& material)
{
    const vector<string> defines = shader->getShaderDefinesFor(material);
    const vector<string> vertexShaderInputs = shader->getVertexShaderInputsFor(material);
    const vector<string> vertexShaderOutputs = shader->getVertexShaderOutputsFor(material);
    const vector<string> fragmentShaderInputs = shader->getFragmentShaderInputsFor(material);


    size_t hashValue = 17;
    hashValue = 31 * hashValue +
                std::hash<string>()(shader->getId());

    for (const string& define : defines) {
        hashValue = 31 * hashValue +
                    std::hash<string>()(define);
    }

    for (const string& input : vertexShaderInputs) {
        hashValue = 31 * hashValue +
                    std::hash<string>()(input);
    }

    for (const string& output : vertexShaderOutputs) {
        hashValue = 31 * hashValue +
                    std::hash<string>()(output);
    }

    for (const string& input : fragmentShaderInputs) {
        hashValue = 31 * hashValue +
                    std::hash<string>()(input);
    }

    return hashValue;
}

// ---------------------------------------------------------------------------------------------------------------------

MaterialShaderPtr MaterialShaderFactory::createShader(const MaterialPtr& material)
{
    string shaderId = material->getShaderId();
    shaderId = shaderId.empty() ? defaultShaderId : shaderId;

    const auto it = allocatorMap.find(shaderId);
    RFX_CHECK_ARGUMENT(it != allocatorMap.end());
    const auto allocator = it->second;

    MaterialShaderPtr shader = allocator();
    shader->create(
        material,
        createMaterialDescriptorSetLayoutFor(material),
        shadersDirectory);

    return shader;
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorSetLayout MaterialShaderFactory::createMaterialDescriptorSetLayoutFor(const MaterialPtr& material)
{
    const VertexFormat& vertexFormat = material->getVertexFormat();
    vector<VkDescriptorSetLayoutBinding> materialDescSetLayoutBindings;
    uint32_t binding = 0;

    materialDescSetLayoutBindings.push_back({
        .binding = binding++,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
    });

    if (vertexFormat.containsTexCoords()) {
        materialDescSetLayoutBindings.push_back({
            .binding = binding++,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        });
    }

    if (vertexFormat.containsTangents()) {
        materialDescSetLayoutBindings.push_back({
            .binding = binding++,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        });
    }

    if (material->getMetallicRoughnessTexture() != nullptr) {
        materialDescSetLayoutBindings.push_back({
            .binding = binding++,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        });
    }

    if (material->getOcclusionTexture() != nullptr) {
        materialDescSetLayoutBindings.push_back({
            .binding = binding++,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        });
    }

    if (material->getEmissiveTexture() != nullptr) {
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

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice->getLogicalDevice(),
        &materialDescSetLayoutCreateInfo,
        nullptr,
        &descriptorSetLayout));

    return descriptorSetLayout;
}

// ---------------------------------------------------------------------------------------------------------------------

void MaterialShaderFactory::addToCache(
    const MaterialShaderPtr& shader,
    const MaterialPtr& material)
{
    const size_t shaderHash = hash(shader, material);

    shaderCache.add(shaderHash, shader);
}

// ---------------------------------------------------------------------------------------------------------------------

void MaterialShaderFactory::clearCache()
{
    shaderCache.clear();
}

// ---------------------------------------------------------------------------------------------------------------------
