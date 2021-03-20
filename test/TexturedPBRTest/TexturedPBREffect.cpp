#include "rfx/pch.h"
#include "TexturedPBREffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

TexturedPBREffect::TexturedPBREffect(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    const shared_ptr<Scene>& scene)
        : TestEffect(graphicsDevice, scene) {}

// ---------------------------------------------------------------------------------------------------------------------

string TexturedPBREffect::getVertexShaderFileName() const
{
    return "pbr_textured.vert";
}

// ---------------------------------------------------------------------------------------------------------------------

string TexturedPBREffect::getFragmentShaderFileName() const
{
    return "pbr_textured.frag";
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBREffect::setProjectionMatrix(const mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBREffect::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (lights_[i] != nullptr) {
            sceneData_.lights[i].position = vec4(lights_[i]->getPosition(), 1.0f);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBREffect::setLight(int index, const shared_ptr<PointLight>& light)
{
    lights_[index] = light;
    if (light == nullptr) {
        sceneData_.lights[index].enabled = false;
        return;
    }

    auto& sceneDataLight = sceneData_.lights[index];
    sceneDataLight.enabled = true;
    sceneDataLight.position = vec4(light->getPosition(), 1.0f);

    const vec3& lightColor = light->getColor();
    sceneDataLight.color = vec4(lightColor * 255.0f, 1.0f);
}
// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBREffect::createMaterialDataBuffers()
{
    for (const auto& material : scene_->getMaterials()) {
        shared_ptr<Buffer> materialUniformBuffer = graphicsDevice_->createBuffer(
            sizeof(MaterialData),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        MaterialData materialData {
            .baseColorFactor = material->getBaseColorFactor(),
            .emissiveFactor = vec4(material->getEmissiveFactor(), 1.0f),
            .metallic = material->getMetallicFactor(),
            .roughness = material->getRoughnessFactor(),
            .baseColorTexCoordSet = material->getBaseColorTexCoordSet(),
            .metallicRoughnessTexCoordSet = material->getMetallicRoughnessTexCoordSet(),
            .normalTexCoordSet = material->getNormalTexCoordSet(),
            .occlusionTexCoordSet = material->getOcclusionTexCoordSet(),
            .occlusionStrength = material->getOcclusionStrength(),
            .emissiveTexCoordSet = material->getEmissiveTexCoordSet()
        };

        graphicsDevice_->bind(materialUniformBuffer);
        materialUniformBuffer->load(sizeof(MaterialData), &materialData);
        materialDataBuffers_.push_back(materialUniformBuffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

size_t TexturedPBREffect::getSceneDataSize() const
{
    return sizeof(SceneData);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBREffect::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBREffect::setCameraPos(const vec3& pos)
{
    sceneData_.camPos = pos;
}

// ---------------------------------------------------------------------------------------------------------------------

VertexFormat TexturedPBREffect::getVertexFormat() const
{
    return scene_->getMaterial(0)->getVertexFormat(); // TODO: support for multiple/different vertex formats
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> TexturedPBREffect::buildShaderDefines(
    const shared_ptr<Material>& material,
    const VertexFormat& vertexFormat)
{
    vector<string> defines;

    if (material->getBaseColorTexture() != nullptr) {
        defines.emplace_back("HAS_BASE_COLOR_MAP 1");
    }

    if (material->getMetallicRoughnessTexture() != nullptr) {
        defines.emplace_back("HAS_METALLIC_ROUGHNESS_MAP 1");
    }

    if (material->getNormalTexture() != nullptr) {
        defines.emplace_back("HAS_NORMAL_MAP 1");
    }

    if (material->getOcclusionTexture() != nullptr) {
        defines.emplace_back("HAS_OCCLUSION_MAP 1");
    }

    if (material->getEmissiveTexture() != nullptr) {
        defines.emplace_back("HAS_EMISSIVE_MAP 1");
    }

    if (vertexFormat.containsNormals()) {
        defines.emplace_back("HAS_NORMALS 1");
    }

    const uint32_t texCoordSetCount = vertexFormat.getTexCoordSetCount();
    if (texCoordSetCount > 0) {
        defines.push_back(fmt::format("TEXCOORDSET_COUNT {}", texCoordSetCount));
    }

    if (vertexFormat.containsTangents()) {
        defines.emplace_back("HAS_TANGENTS 1");
    }

    return defines;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> TexturedPBREffect::buildVertexShaderInputs(const VertexFormat& vertexFormat)
{
    vector<string> inputs;

    // location=0 for coordinates (must be always present)
    uint32_t location = 1;

    if (vertexFormat.containsNormals()) {
        inputs.push_back(fmt::format("layout(location = {}) in vec3 inNormal;", location));
        location++;
    }

    uint32_t texCoordSetCount = vertexFormat.getTexCoordSetCount();
    if (texCoordSetCount > 0) {
        inputs.push_back(fmt::format("layout(location = {}) in vec2 inTexCoord[{}];", location, texCoordSetCount));
        location += texCoordSetCount;
    }

    if (vertexFormat.containsTangents()) {
        inputs.push_back(fmt::format("layout(location = {}) in vec4 inTangent;", location));
        location++;
    }

    return inputs;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> TexturedPBREffect::buildVertexShaderOutputs(const VertexFormat& vertexFormat)
{
    vector<string> outputs;

    // location=0 for coordinates (must be always present)
    uint32_t location = 1;

    if (vertexFormat.containsNormals()) {
        outputs.push_back(fmt::format("layout(location = {}) out vec3 outNormal;", location));
        location++;
    }

    uint32_t texCoordSetCount = vertexFormat.getTexCoordSetCount();
    if (texCoordSetCount > 0) {
        outputs.push_back(fmt::format("layout(location = {}) out vec2 outTexCoord[{}];", location, texCoordSetCount));
        location += texCoordSetCount;
    }

    if (vertexFormat.containsTangents()) {
        outputs.push_back(fmt::format("layout(location = {}) out vec3 outTangentCamPos;", location++));
        outputs.push_back(fmt::format("layout(location = {}) out vec3 outTangentPosition;", location++));
        outputs.push_back(fmt::format("layout(location = {}) out vec3 outTangentLightPos[{}];", location, MAX_LIGHTS));
        location += MAX_LIGHTS;
    }

    return outputs;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> TexturedPBREffect::buildFragmentShaderInputs(const VertexFormat& vertexFormat)
{
    vector<string> inputs;

    // location=0 for coordinates (must be always present)
    uint32_t location = 1;

    if (vertexFormat.containsNormals()) {
        inputs.push_back(fmt::format("layout(location = {}) in vec3 inNormal;", location));
        location++;
    }

    uint32_t texCoordSetCount = vertexFormat.getTexCoordSetCount();
    if (texCoordSetCount > 0) {
        inputs.push_back(fmt::format("layout(location = {}) in vec2 inTexCoord[{}];", location, texCoordSetCount));
        location += texCoordSetCount;
    }

    if (vertexFormat.containsTangents()) {
        inputs.push_back(fmt::format("layout(location = {}) in vec3 inTangentCamPos;", location));
        location++;
        inputs.push_back(fmt::format("layout(location = {}) in vec3 inTangentPosition;", location));
        location++;
        inputs.push_back(fmt::format("layout(location = {}) in vec3 inTangentLightPos[{}];", location, MAX_LIGHTS));
        location += MAX_LIGHTS;
    }

    return inputs;
}

// ---------------------------------------------------------------------------------------------------------------------
