#include "rfx/pch.h"
#include "rfx/scene/MaterialShader.h"


using namespace rfx;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

MaterialShader::MaterialShader(
    GraphicsDevicePtr graphicsDevice,
    string id,
    string vertexShaderId,
    string fragmentShaderId)
    : graphicsDevice(move(graphicsDevice)),
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
    VkDevice device = graphicsDevice->getLogicalDevice();

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

void MaterialShader::create(
    ShaderProgramPtr shaderProgram,
    VkDescriptorSetLayout shaderDescriptorSetLayout,
    VkDescriptorSet shaderDescriptorSet,
    BufferPtr shaderDataBuffer,
    VkDescriptorSetLayout materialDescriptorSetLayout)
{
    this->shaderProgram = move(shaderProgram);
    this->shaderDescriptorSetLayout = shaderDescriptorSetLayout;
    this->shaderDescriptorSet = shaderDescriptorSet;
    this->shaderDataBuffer = move(shaderDataBuffer);
    this->materialDescriptorSetLayout = materialDescriptorSetLayout;
}

// ---------------------------------------------------------------------------------------------------------------------

const string& MaterialShader::getVertexShaderId() const
{
    return vertexShaderId;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<VertexShader>& MaterialShader::getVertexShader() const
{
    return shaderProgram->getVertexShader();
}

// ---------------------------------------------------------------------------------------------------------------------

const string& MaterialShader::getFragmentShaderId() const
{
    return fragmentShaderId;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<FragmentShader>& MaterialShader::getFragmentShader() const
{
    return shaderProgram->getFragmentShader();
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
