#include "rfx/pch.h"
#include "rfx/scene/MaterialShaderFactory.h"


using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

MaterialShaderFactory::MaterialShaderFactory(
    std::filesystem::path shadersDirectory,
    std::string defaultShaderId)
    : shadersDirectory(move(shadersDirectory)),
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
    string shaderId = material->getShaderId();
    shaderId = shaderId.empty() ? defaultShaderId : shaderId;

    const auto it = allocatorMap.find(shaderId);
    RFX_CHECK_ARGUMENT(it != allocatorMap.end());

    const auto allocator = it->second;
    MaterialShaderPtr shader = allocator();
    size_t shaderHash = hash(shader, material);

    MaterialShaderPtr cachedShader = shaderCache.get(shaderHash);
    if (cachedShader != nullptr) {
        return cachedShader;
    }

    shader->loadShaders(material, shadersDirectory);

    shaderCache.add(shaderHash, shader);

    return shader;
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

void MaterialShaderFactory::clearCache()
{
    shaderCache.clear();
}

// ---------------------------------------------------------------------------------------------------------------------
