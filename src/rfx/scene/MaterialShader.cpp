#include "rfx/pch.h"
#include "rfx/scene/MaterialShader.h"
#include "rfx/graphics/ShaderLoader.h"
#include "rfx/common/Logger.h"


using namespace rfx;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

MaterialShader::MaterialShader(
    GraphicsDevicePtr graphicsDevice,
    std::string id,
    std::string vertexShaderId,
    std::string fragmentShaderId)
    : graphicsDevice_(move(graphicsDevice)),
      id(move(id)),
      vertexShaderId(move(vertexShaderId)),
      fragmentShaderId(move(fragmentShaderId)) {}

// ---------------------------------------------------------------------------------------------------------------------

MaterialShader::~MaterialShader()
{
    destroy();
}

// ---------------------------------------------------------------------------------------------------------------------

void MaterialShader::destroy()
{
    VkDevice device = graphicsDevice_->getLogicalDevice();

    if (materialDescriptorSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(device, materialDescriptorSetLayout, nullptr);
        materialDescriptorSetLayout = VK_NULL_HANDLE;
    }

    if (shaderDescriptorSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(device, shaderDescriptorSetLayout, nullptr);
        shaderDescriptorSetLayout = VK_NULL_HANDLE;
    }

    if (pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(device, pipeline, nullptr);
        pipeline = VK_NULL_HANDLE;
    }

    if (pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        pipelineLayout = VK_NULL_HANDLE;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const string& MaterialShader::getId() const
{
    return id;
}

// ---------------------------------------------------------------------------------------------------------------------

// TODO: move to factory (?!)
void MaterialShader::create(
    const MaterialPtr& material,
    VkDescriptorSetLayout materialDescriptorSetLayout,
    const path& shadersDirectory)
{
    const path vertexShaderFilename = vertexShaderId + ".vert";
    const path fragmentShaderFilename = fragmentShaderId + ".frag";
    const VertexFormat& vertexFormat = material->getVertexFormat();

    const vector<string> defines = getShaderDefinesFor(material);
    const vector<string> vertexShaderInputs = getVertexShaderInputsFor(material);
    const vector<string> vertexShaderOutputs = getVertexShaderOutputsFor(material);
    const vector<string> fragmentShaderInputs = getFragmentShaderInputsFor(material);


    const ShaderLoader shaderLoader(graphicsDevice_);
    RFX_LOG_INFO << "Loading vertex shader ...";
    vertexShader = shaderLoader.loadVertexShader(
        shadersDirectory / vertexShaderFilename,
        "main",
        vertexFormat,
        defines,
        vertexShaderInputs,
        vertexShaderOutputs);

    RFX_LOG_INFO << "Loading fragment shader ...";
    fragmentShader = shaderLoader.loadFragmentShader(
        shadersDirectory / fragmentShaderFilename,
        "main",
        defines,
        fragmentShaderInputs);

    this->materialDescriptorSetLayout = materialDescriptorSetLayout;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<VertexShader>& MaterialShader::getVertexShader() const
{
    return vertexShader;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<FragmentShader>& MaterialShader::getFragmentShader() const
{
    return fragmentShader;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> MaterialShader::getShaderDefinesFor(const MaterialPtr& material)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> MaterialShader::getVertexShaderInputsFor(const MaterialPtr& material)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> MaterialShader::getVertexShaderOutputsFor(const MaterialPtr& material)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> MaterialShader::getFragmentShaderInputsFor(const MaterialPtr& material)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorSetLayout MaterialShader::getMaterialDescriptorSetLayout() const
{
    return materialDescriptorSetLayout;
}

// ---------------------------------------------------------------------------------------------------------------------

void MaterialShader::setPipeline(VkPipelineLayout pipelineLayout, VkPipeline pipeline)
{
    this->pipelineLayout = pipelineLayout;
    this->pipeline = pipeline;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineLayout MaterialShader::getPipelineLayout() const
{
    return pipelineLayout;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipeline MaterialShader::getPipeline() const
{
    return pipeline;
}

// ---------------------------------------------------------------------------------------------------------------------

void MaterialShader::setResources(
    VkDescriptorSetLayout descriptorSetLayout,
    VkDescriptorSet descriptorSet,
    BufferPtr buffer)
{
    shaderDescriptorSetLayout = descriptorSetLayout;
    shaderDescriptorSet = descriptorSet;
    shaderDataBuffer = move(buffer);
}

// ---------------------------------------------------------------------------------------------------------------------

void MaterialShader::updateDataBuffer()
{
    const uint32_t dataSize = getDataSize();

    if (dataSize > 0) {
        shaderDataBuffer->load(dataSize, getData());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorSetLayout MaterialShader::getShaderDescriptorSetLayout() const
{
    return shaderDescriptorSetLayout;
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorSet MaterialShader::getShaderDescriptorSet() const
{
    return shaderDescriptorSet;
}

// ---------------------------------------------------------------------------------------------------------------------
