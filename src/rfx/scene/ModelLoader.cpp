#include "rfx/pch.h"
#include "rfx/scene/ModelLoader.h"
#include "rfx/scene/PointLight.h"
#include "rfx/scene/SpotLight.h"

#include <nlohmann/json.hpp>
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>


using namespace rfx;
using namespace glm;
using namespace std;
using namespace filesystem;

static const string GLTF_DIRECTIONAL_LIGHT_TYPE = "directional";
static const string GLTF_POINT_LIGHT_TYPE = "point";
static const string GLTF_SPOT_LIGHT_TYPE = "spot";

// ---------------------------------------------------------------------------------------------------------------------

struct GLTFLightProperties {
    string type;
    string name;
    vec3 color { 1.0f };
    int intensity = 1;
    float range;
    float innerConeAngle = 0.0f;
    float outerConeAngle = radians(90.0f);
};

// ---------------------------------------------------------------------------------------------------------------------

class ModelLoader::ModelLoaderImpl
{
public:
    explicit ModelLoaderImpl(
        shared_ptr<GraphicsDevice>&& graphicsDevice)
            : graphicsDevice_(move(graphicsDevice)) {}

    const shared_ptr<Model>& load(const path& scenePath);
    void clear();
    VertexFormat getVertexFormatFrom(const tinygltf::Primitive& firstPrimitive);
    void checkVertexFormatConsistency();
    void loadImages();
    static vector<std::byte> convertToRGBA(const tinygltf::Image& gltfImage);
    void loadSamplers();
    void loadTextures();
    void loadTexture(const tinygltf::Texture& gltfTexture);
    void loadMaterials();
    void loadMaterial(const tinygltf::Material& glTFMaterial) const;
    void loadMeshes();
    void prepareGeometryBuffers();
    void loadMesh(const tinygltf::Mesh& gltfMesh);
    void loadVertices(const tinygltf::Primitive& glTFPrimitive);
    const float* getBufferData(const tinygltf::Primitive& glTFPrimitive, const string& attribute);
    void appendVertexData(
        uint32_t vertexCount,
        const float* positionBuffer,
        const float* normalsBuffer,
        const float** texCoordsBuffers,
        const float* tangentsBuffer);
    uint32_t appendCoordinates(const float* positionBuffer, uint32_t vertexIndex, uint32_t destIndex);
    uint32_t appendNormals(const float* normalsBuffer, uint32_t vertexIndex, uint32_t destIndex);
    uint32_t appendTexCoords(
        const float** texCoordsBuffers,
        uint32_t vertexIndex,
        uint32_t destIndex);
    uint32_t appendTangents(const float* tangentsBuffer, uint32_t vertexIndex, uint32_t destIndex);
    uint32_t loadIndices(const tinygltf::Primitive& glTFPrimitive, uint32_t vertexStart);
    void loadLights();
    void loadLight(const tinygltf::Value::Object& gltfLight);
    shared_ptr<Light> loadPointLight(const tinygltf::Value::Object& gltfLight);
    GLTFLightProperties getLightProperties(const tinygltf::Value::Object& gltfLight);
    shared_ptr<Light> loadSpotLight(const tinygltf::Value::Object& gltfLight);
    void loadNodes();
    void loadNode(
        const tinygltf::Node& node,
        const shared_ptr<ModelNode>& parentNode);
    static mat4 getLocalTransformOf(const tinygltf::Node& gltfNode) ;

    void buildVertexBuffer();
    void buildIndexBuffer();

    shared_ptr<GraphicsDevice> graphicsDevice_;
    tinygltf::Model gltfModel_;
    VertexFormat vertexFormat_;
    shared_ptr<Model> scene_;
    uint32_t vertexCount_ = 0;
    vector<float> vertexData_;
    vector<uint32_t> indices_;
    vector<SamplerDesc> samplers_;
    vector<shared_ptr<Image>> images_;
};

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Model>& ModelLoader::ModelLoaderImpl::load(const path& scenePath)
{
    RFX_CHECK_STATE(exists(scenePath), "File not found: " + scenePath.string());

    clear();


    tinygltf::TinyGLTF gltfContext;
    string error;
    string warning;

    bool result = gltfContext.LoadASCIIFromFile(&gltfModel_, &error, &warning, scenePath.string());
    RFX_CHECK_STATE(result,
        "Failed to load glTF file: " + scenePath.string() + "\n"
        + "Errors: " + error
        + "Warnings: " + warning);

    RFX_CHECK_STATE(!gltfModel_.meshes.empty(), "No meshes");
    RFX_CHECK_STATE(!gltfModel_.meshes[0].primitives.empty(), "Empty mesh");

    vertexFormat_ = getVertexFormatFrom(gltfModel_.meshes[0].primitives[0]);
    checkVertexFormatConsistency();

    loadImages();
    loadSamplers();
    loadTextures();
    loadMaterials();
    loadMeshes();
    loadLights();
    loadNodes();
    buildVertexBuffer();
    buildIndexBuffer();

    scene_->compile();

    return scene_;
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::clear()
{
    gltfModel_ = {};

    scene_.reset();
    scene_ = make_shared<Model>();
    vertexCount_ = 0;
    vertexData_.clear();
    indices_.clear();
    samplers_.clear();
    images_.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

VertexFormat ModelLoader::ModelLoaderImpl::getVertexFormatFrom(const tinygltf::Primitive& firstPrimitive)
{
    unsigned int formatMask = 0;
    unsigned int texCoordSetCount = 0;

    if (firstPrimitive.attributes.contains("POSITION")) {
        formatMask |= VertexFormat::COORDINATES;
    }
    if (firstPrimitive.attributes.contains("NORMAL")) {
        formatMask |= VertexFormat::NORMALS;
    }
    if (firstPrimitive.attributes.contains("TEXCOORD_0")) {
        formatMask |= VertexFormat::TEXCOORDS;
        texCoordSetCount = 1;
    }
    if (firstPrimitive.attributes.contains("TEXCOORD_1")) {
        texCoordSetCount++;
    }
    if (firstPrimitive.attributes.contains("TEXCOORD_2")) {
        texCoordSetCount++;
    }
    if (firstPrimitive.attributes.contains("TEXCOORD_3")) {
        texCoordSetCount++;
    }
    if (firstPrimitive.attributes.contains("TEXCOORD_4")) {
        texCoordSetCount++;
    }
    if (firstPrimitive.attributes.contains("TANGENT")) {
        formatMask |= VertexFormat::TANGENTS;
    }

    return VertexFormat(formatMask, texCoordSetCount);
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::checkVertexFormatConsistency()
{
    for (const auto& mesh : gltfModel_.meshes) {
        for (const auto& primitive : mesh.primitives) {
            RFX_CHECK_STATE(getVertexFormatFrom(primitive) == vertexFormat_,
                "different vertex formats aren't supported yet");
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::loadImages()
{
    for (const auto& gltfImage : gltfModel_.images) {
        vector<std::byte> imageData;

        if (gltfImage.component == 3) {
            imageData = convertToRGBA(gltfImage);
        }
        else {
            VkDeviceSize imageDataSize = gltfImage.image.size();
            imageData.resize(imageDataSize);
            memcpy(imageData.data(), gltfImage.image.data(), imageDataSize);
        }

        ImageDesc imageDesc = {
            .format = VK_FORMAT_R8G8B8A8_UNORM,
            .width = static_cast<uint32_t>(gltfImage.width),
            .height = static_cast<uint32_t>(gltfImage.height),
            .bytesPerPixel = 4,
            .mipLevels = 1,
            .mipOffsets = { 0 }
        };

        const shared_ptr<Image> image = graphicsDevice_->createImage(gltfImage.name, imageDesc, imageData, false);
        images_.push_back(image);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

vector<std::byte> ModelLoader::ModelLoaderImpl::convertToRGBA(const tinygltf::Image& gltfImage)
{
    const unsigned char* rgbData = gltfImage.image.data();

    size_t rgbaDataSize = gltfImage.width * gltfImage.height * 4;
    vector<std::byte> rgbaData(rgbaDataSize);

    size_t rgbIndex = 0;
    size_t rgbaIndex = 0;

    for (size_t i = 0; i < gltfImage.width * gltfImage.height; ++i) {
        rgbaData[rgbaIndex + 0] = static_cast<std::byte>(rgbData[rgbIndex + 0]);
        rgbaData[rgbaIndex + 1] = static_cast<std::byte>(rgbData[rgbIndex + 1]);
        rgbaData[rgbaIndex + 2] = static_cast<std::byte>(rgbData[rgbIndex + 2]);
        rgbaData[rgbaIndex + 3] = static_cast<std::byte>(255);

        rgbIndex += 3;
        rgbaIndex += 4;
    }

    return rgbaData;
}

// ---------------------------------------------------------------------------------------------------------------------

VkFilter toFilter(int gl_value)
{
    return gl_value == GL_NEAREST
        ? VK_FILTER_NEAREST
        : VK_FILTER_LINEAR;
}

// ---------------------------------------------------------------------------------------------------------------------

VkSamplerMipmapMode toMipMapMode(int gl_value)
{
    switch (gl_value) {
    case GL_NEAREST:
    case GL_LINEAR:
        return static_cast<VkSamplerMipmapMode>(0);
    case GL_NEAREST_MIPMAP_NEAREST:
    case GL_LINEAR_MIPMAP_NEAREST:
    case GL_NEAREST_MIPMAP_LINEAR:
        return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    default:
        return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

SamplerDesc toSamplerDesc(const tinygltf::Sampler& gltfSampler)
{
    return {
        .minFilter = toFilter(gltfSampler.minFilter),
        .magFilter = toFilter(gltfSampler.magFilter),
        .mipmapMode = toMipMapMode(gltfSampler.minFilter)
    };
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::loadSamplers()
{
    ranges::transform(gltfModel_.samplers, back_inserter(samplers_), toSamplerDesc);
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::loadTextures()
{
    for (const auto& texture : gltfModel_.textures) {
        loadTexture(texture);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::loadTexture(const tinygltf::Texture& gltfTexture)
{
    const shared_ptr<Image>& image = images_[gltfTexture.source];
    const ImageDesc imageDesc = image->getDesc();
    VkImageView imageView = graphicsDevice_->createImageView(
        image,
        imageDesc.format,
        VK_IMAGE_ASPECT_COLOR_BIT,
        imageDesc.mipLevels);
    const SamplerDesc& samplerDesc = samplers_[gltfTexture.sampler];

    scene_->addTexture(graphicsDevice_->createTexture2D(image, imageView, samplerDesc));
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::loadMaterials()
{
    for (const auto& gltfMaterial : gltfModel_.materials) {
        loadMaterial(gltfMaterial);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::loadMaterial(const tinygltf::Material& glTFMaterial) const
{
    tinygltf::Value::Object extras = glTFMaterial.extras.Get<tinygltf::Value::Object>();
    const string shaderId = extras.contains("shader")
            ? extras.at("shader").Get<string>()
            : "";

    const auto material = make_shared<Material>(
        glTFMaterial.name,
        vertexFormat_,
        shaderId);

    // TODO: specular-glossiness material model
    const tinygltf::PbrMetallicRoughness& gltfMetallicRoughness = glTFMaterial.pbrMetallicRoughness;

    material->setBaseColorFactor(make_vec4(gltfMetallicRoughness.baseColorFactor.data()));
    if (gltfMetallicRoughness.baseColorTexture.index > -1) {
        material->setBaseColorTexture(
            scene_->getTexture(gltfMetallicRoughness.baseColorTexture.index),
            gltfMetallicRoughness.baseColorTexture.texCoord);
    }

    if (gltfMetallicRoughness.metallicRoughnessTexture.index > -1) {
        material->setMetallicRoughnessTexture(
            scene_->getTexture(gltfMetallicRoughness.metallicRoughnessTexture.index),
            gltfMetallicRoughness.metallicRoughnessTexture.texCoord);
    }
    material->setMetallicFactor(static_cast<float>(gltfMetallicRoughness.metallicFactor));
    material->setRoughnessFactor(static_cast<float>(gltfMetallicRoughness.roughnessFactor));

    if (glTFMaterial.normalTexture.index > -1) {
        material->setNormalTexture(scene_->getTexture(glTFMaterial.normalTexture.index),
        glTFMaterial.normalTexture.texCoord);
    }

    if (glTFMaterial.occlusionTexture.index > -1) {
        material->setOcclusionTexture(
            scene_->getTexture(glTFMaterial.occlusionTexture.index),
            glTFMaterial.occlusionTexture.texCoord,
            static_cast<float>(glTFMaterial.occlusionTexture.strength));
    }

    if (glTFMaterial.emissiveTexture.index > -1) {
        material->setEmissiveFactor(make_vec3(glTFMaterial.emissiveFactor.data()));
        material->setEmissiveTexture(
            scene_->getTexture(glTFMaterial.emissiveTexture.index),
            glTFMaterial.emissiveTexture.texCoord);
    }

    scene_->addMaterial(material);
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::loadMeshes()
{
    prepareGeometryBuffers();

    for (const auto& gltfMesh : gltfModel_.meshes) {
        loadMesh(gltfMesh);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::prepareGeometryBuffers()
{
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;

    for (const auto& gltfMesh : gltfModel_.meshes) {
        for (const auto& gltfPrimitive : gltfMesh.primitives) {
            RFX_CHECK_STATE(gltfPrimitive.mode == TINYGLTF_MODE_TRIANGLES, "");

            const auto& accessor = gltfModel_.accessors[gltfPrimitive.attributes.find("POSITION")->second];
            vertexCount += static_cast<uint32_t>(accessor.count);

            if (gltfPrimitive.indices > -1) {
                const auto& indexAccessor = gltfModel_.accessors[gltfPrimitive.indices];
                indexCount += static_cast<uint32_t>(indexAccessor.count);
            }
            else {
                indexCount += static_cast<uint32_t>(accessor.count);
            }
        }
    }

    vertexData_.resize(vertexCount * (vertexFormat_.getVertexSize() / sizeof(float)));
    indices_.reserve(indexCount);
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::loadMesh(const tinygltf::Mesh& gltfMesh)
{
    auto mesh = make_unique<Mesh>();

    for (const tinygltf::Primitive& glTFPrimitive : gltfMesh.primitives) {

        auto firstIndex = static_cast<uint32_t>(indices_.size());
        auto vertexStart = vertexCount_;

        loadVertices(glTFPrimitive);
        uint32_t indexCount = loadIndices(glTFPrimitive, vertexStart);

        mesh->addSubMesh({
            firstIndex,
            indexCount,
            scene_->getMaterial(glTFPrimitive.material)
        });
    }

    scene_->addMesh(move(mesh));
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::loadVertices(const tinygltf::Primitive& glTFPrimitive)
{
    const tinygltf::Accessor& accessor = gltfModel_.accessors[glTFPrimitive.attributes.find("POSITION")->second];
    const uint32_t vertexCount = accessor.count;

    // POSITION
    const float* positionBuffer = getBufferData(glTFPrimitive, "POSITION");

    // COLOR_0
    const float* colorsBuffer = getBufferData(glTFPrimitive, "COLOR_0");
    RFX_CHECK_STATE(colorsBuffer == nullptr, "Colors not implemented yet!");

    // NORMAL
    const float* normalsBuffer = nullptr;
    if (vertexFormat_.containsNormals()) {
        normalsBuffer = getBufferData(glTFPrimitive, "NORMAL");
        RFX_CHECK_STATE(normalsBuffer != nullptr, "Tangents generation not implemented yet!");
    }

    // TEXCOORD
    uint32_t inputTexCoordSetCount = 0;
    uint32_t outputTexCoordSetCount = vertexFormat_.getTexCoordSetCount();
    for (uint32_t i = 0; i < outputTexCoordSetCount; ++i) {
        const string attributeName = "TEXCOORD_" + to_string(i);
        if (!glTFPrimitive.attributes.contains(attributeName)) {
            break;
        }
        inputTexCoordSetCount++;
    }
    RFX_CHECK_STATE(inputTexCoordSetCount <= outputTexCoordSetCount,
        "TexCoordSet count mismatch! Input file contains " + to_string(inputTexCoordSetCount) + " sets.");

    const float* texCoordsBuffers[VertexFormat::MAX_TEXCOORDSET_COUNT] {};
    for (uint32_t i = 0; i < inputTexCoordSetCount; ++i) {
        const string attributeName = "TEXCOORD_" + to_string(i);
        texCoordsBuffers[i] = getBufferData(glTFPrimitive, attributeName);
        RFX_CHECK_STATE(texCoordsBuffers[i] != nullptr, "Missing TexCoordSet in input file!");
    }

    // TANGENT
    const float* tangentsBuffer = nullptr;
    if (vertexFormat_.containsTangents()) {
        tangentsBuffer = getBufferData(glTFPrimitive, "TANGENT");
        RFX_CHECK_STATE(tangentsBuffer != nullptr, "Tangents generation not implemented yet!");
    }

    appendVertexData(
        vertexCount,
        positionBuffer,
        normalsBuffer,
        texCoordsBuffers,
        tangentsBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

const float* ModelLoader::ModelLoaderImpl::getBufferData(
    const tinygltf::Primitive& glTFPrimitive,
    const string& attribute)
{
    if (!glTFPrimitive.attributes.contains(attribute)) {
        return nullptr;
    };

    const tinygltf::Accessor& accessor = gltfModel_.accessors[glTFPrimitive.attributes.find(attribute)->second];
    const tinygltf::BufferView& view = gltfModel_.bufferViews[accessor.bufferView];

    return reinterpret_cast<const float*>(&(gltfModel_.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::appendVertexData(
    uint32_t vertexCount,
    const float* positionBuffer,
    const float* normalsBuffer,
    const float** texCoordsBuffers,
    const float* tangentsBuffer)
{
    uint32_t destIndex = vertexCount_ * (vertexFormat_.getVertexSize() / sizeof(float));

    for (uint32_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
        destIndex += appendCoordinates(positionBuffer, vertexIndex, destIndex);
        destIndex += appendNormals(normalsBuffer, vertexIndex, destIndex);
        destIndex += appendTexCoords(texCoordsBuffers, vertexIndex, destIndex);
        destIndex += appendTangents(tangentsBuffer, vertexIndex, destIndex);
    }

    vertexCount_ += vertexCount;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ModelLoader::ModelLoaderImpl::appendCoordinates(
    const float* positionBuffer,
    uint32_t vertexIndex,
    uint32_t destIndex)
{
    memcpy(&this->vertexData_[destIndex], &positionBuffer[vertexIndex * 3], sizeof(vec3));

    return 3;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ModelLoader::ModelLoaderImpl::appendNormals(
    const float* normalsBuffer,
    uint32_t vertexIndex,
    uint32_t destIndex)
{
    if (normalsBuffer) {
        vec3 normal = normalize(make_vec3(&normalsBuffer[vertexIndex * 3]));
        memcpy(&vertexData_[destIndex], &normal, sizeof(vec3));

        return 3;
    }

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ModelLoader::ModelLoaderImpl::appendTexCoords(
    const float** texCoordsBuffers,
    const uint32_t vertexIndex,
    const uint32_t destIndex)
{
    uint32_t offset = 0;

    for (uint32_t i = 0; i < VertexFormat::MAX_TEXCOORDSET_COUNT; ++i) {
        if (texCoordsBuffers[i] == nullptr) {
            break;
        }

        memcpy(&vertexData_[destIndex + offset], &texCoordsBuffers[i][vertexIndex * 2], sizeof(vec2));
        offset += 2;
    }

    return offset;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ModelLoader::ModelLoaderImpl::appendTangents(
    const float* tangentsBuffer,
    uint32_t vertexIndex,
    uint32_t destIndex)
{
    if (tangentsBuffer) {
        memcpy(&vertexData_[destIndex], &tangentsBuffer[vertexIndex * 4], sizeof(vec4));
        return 4;
    }

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ModelLoader::ModelLoaderImpl::loadIndices(
    const tinygltf::Primitive& glTFPrimitive,
    uint32_t vertexStart)
{
    const tinygltf::Accessor& accessor = gltfModel_.accessors[glTFPrimitive.indices];
    const tinygltf::BufferView& bufferView = gltfModel_.bufferViews[accessor.bufferView];
    const tinygltf::Buffer& buffer = gltfModel_.buffers[bufferView.buffer];

    auto indexCount = static_cast<uint32_t>(accessor.count);

    // glTF supports different component types of indices_
    switch (accessor.componentType) {
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
            vector<uint32_t> buf(accessor.count);
            memcpy(&buf[0], &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
            for (size_t index = 0; index < accessor.count; index++) {
                indices_.push_back(buf[index] + vertexStart);
            }
            break;
        }
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
            vector<uint16_t> buf(accessor.count);
            memcpy(&buf[0], &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
            for (size_t index = 0; index < accessor.count; index++) {
                indices_.push_back(buf[index] + vertexStart);
            }
            break;
        }
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
            vector<uint8_t> buf(accessor.count);
            memcpy(&buf[0], &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
            for (size_t index = 0; index < accessor.count; index++) {
                indices_.push_back(buf[index] + vertexStart);
            }
            break;
        }
        default:
            RFX_THROW("Index component type "s + to_string(accessor.componentType) + " not supported!"s);
    }

    return indexCount;
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::loadLights()
{
    if (!gltfModel_.extensions.contains("KHR_lights_punctual")) {
        return;
    }

    tinygltf::Value lightsExtension = gltfModel_.extensions["KHR_lights_punctual"];
    auto& gltfLights = lightsExtension.Get<tinygltf::Value::Object>()["lights"].Get<tinygltf::Value::Array>();
    for (const auto& gltfLight : gltfLights) {
        loadLight(gltfLight.Get<tinygltf::Value::Object>());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::loadLight(const tinygltf::Value::Object& gltfLight)
{
    shared_ptr<Light> light;

    const string type = gltfLight.find("type")->second.Get<string>();
    if (type == GLTF_POINT_LIGHT_TYPE) {
        light = loadPointLight(gltfLight);
    }
    else if (type == GLTF_SPOT_LIGHT_TYPE) {
        light = loadSpotLight(gltfLight);
    }
    else  {
        RFX_THROW_NOT_IMPLEMENTED();
    }

    scene_->addLight(light);
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Light> ModelLoader::ModelLoaderImpl::loadPointLight(const tinygltf::Value::Object& gltfLight)
{
    const GLTFLightProperties lightProperties = getLightProperties(gltfLight);

    auto pointLight = make_shared<PointLight>(lightProperties.name);
    pointLight->setColor(lightProperties.color);
    // TODO: consider intensity


    return pointLight;
}

// ---------------------------------------------------------------------------------------------------------------------

GLTFLightProperties ModelLoader::ModelLoaderImpl::getLightProperties(const tinygltf::Value::Object& gltfLight)
{
    GLTFLightProperties lightProperties;

    if (gltfLight.contains("name")) {
        lightProperties.name = gltfLight.at("name").Get<string>();
    }

    if (gltfLight.contains("color")) {
        const auto& gltfColor = gltfLight.at("color").Get<tinygltf::Value::Array>();
        lightProperties.color = vec3(
            gltfColor[0].GetNumberAsDouble(),
            gltfColor[1].GetNumberAsDouble(),
            gltfColor[2].GetNumberAsDouble()
        );
    }

    if (gltfLight.contains("range")) {
        RFX_THROW_NOT_IMPLEMENTED();
    }

    if (gltfLight.contains("intensity")) {
        lightProperties.intensity = gltfLight.at("intensity").GetNumberAsInt();
    }

    if (gltfLight.contains("spot")) {
        auto& gltfSpot = gltfLight.find("spot")->second.Get<tinygltf::Value::Object>();
        lightProperties.innerConeAngle = static_cast<float>(gltfSpot.at("innerConeAngle").GetNumberAsDouble());
        lightProperties.outerConeAngle = static_cast<float>(gltfSpot.at("outerConeAngle").GetNumberAsDouble());
    }

    return lightProperties;
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Light> ModelLoader::ModelLoaderImpl::loadSpotLight(const tinygltf::Value::Object& gltfLight)
{
    const GLTFLightProperties lightProperties = getLightProperties(gltfLight);

    auto spotLight = make_shared<SpotLight>(lightProperties.name);
    spotLight->setColor(lightProperties.color);
    // TODO: consider intensity
    spotLight->setInnerConeAngle(glm::degrees(lightProperties.innerConeAngle));
    spotLight->setOuterConeAngle(glm::degrees(lightProperties.outerConeAngle));

    return spotLight;
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::loadNodes()
{
    RFX_CHECK_STATE(gltfModel_.scenes.size() == 1, "Multiple scenes not supported yet");

    const tinygltf::Scene& gltfScene = gltfModel_.scenes[0];
    for (const auto nodeIndex : gltfScene.nodes) {
        const tinygltf::Node& node = gltfModel_.nodes[nodeIndex];
        loadNode(node, scene_->getRootNode());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::loadNode(
    const tinygltf::Node& gltfNode,
    const shared_ptr<ModelNode>& parentNode)
{
    auto node = make_shared<ModelNode>(parentNode);
    node->setLocalTransform(getLocalTransformOf(gltfNode));
    if (gltfNode.mesh > -1) {
        node->addMesh(scene_->getMesh(gltfNode.mesh));
    }
    if (gltfNode.extensions.contains("KHR_lights_punctual")) {
        const auto& lightExtension = gltfNode.extensions.find("KHR_lights_punctual")->second.Get<tinygltf::Value::Object>();
        int lightIndex = lightExtension.find("light")->second.GetNumberAsInt();
        node->addLight(scene_->getLight(lightIndex));
    }
    parentNode->addChild(node);

    for (int childIndex : gltfNode.children) {
        loadNode(gltfModel_.nodes[childIndex], node);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

mat4 ModelLoader::ModelLoaderImpl::getLocalTransformOf(const tinygltf::Node& gltfNode)
{
    mat4 translation { 1.0f };
    mat4 scale { 1.0f };
    mat4 rotation { 1.0f };
    mat4 matrix { 1.0f };

    if (!gltfNode.translation.empty()) {
        translation = translate(translation, vec3(make_vec3(gltfNode.translation.data())));
    }

    if (!gltfNode.scale.empty()) {
        scale = glm::scale(scale, vec3(make_vec3(gltfNode.scale.data())));
    }

    if (!gltfNode.rotation.empty()) {
        quat q = make_quat(gltfNode.rotation.data());
        rotation *= mat4(q);
    }

    if (!gltfNode.matrix.empty()) {
        matrix = make_mat4x4(gltfNode.matrix.data());
    }

    return translation * rotation * scale * matrix;
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::buildVertexBuffer()
{
    const size_t vertexDataSize = vertexData_.size() * sizeof(float);

    shared_ptr<VertexBuffer> vertexBuffer = graphicsDevice_->createVertexBuffer(vertexCount_, vertexFormat_);
    scene_->setVertexBuffer(vertexBuffer);
    graphicsDevice_->bind(vertexBuffer);

    shared_ptr<Buffer> stagingBuffer = graphicsDevice_->createBuffer(
        vertexDataSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* mappedMemory = nullptr;
    graphicsDevice_->bind(stagingBuffer);
    graphicsDevice_->map(stagingBuffer, &mappedMemory);
    memcpy(mappedMemory, vertexData_.data(), vertexDataSize);
    graphicsDevice_->unmap(stagingBuffer);

    VkCommandPool graphicsCommandPool = graphicsDevice_->getGraphicsCommandPool();
    shared_ptr<CommandBuffer> commandBuffer = graphicsDevice_->createCommandBuffer(graphicsCommandPool);
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    commandBuffer->copyBuffer(stagingBuffer, vertexBuffer);
    commandBuffer->end();

    graphicsDevice_->getGraphicsQueue()->flush(commandBuffer);

    graphicsDevice_->destroyCommandBuffer(commandBuffer, graphicsCommandPool);
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::ModelLoaderImpl::buildIndexBuffer()
{
    const VkDeviceSize bufferSize = indices_.size() * sizeof(uint32_t);
    shared_ptr<Buffer> stagingBuffer = graphicsDevice_->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* mappedMemory = nullptr;
    graphicsDevice_->bind(stagingBuffer);
    graphicsDevice_->map(stagingBuffer, &mappedMemory);
    memcpy(mappedMemory, indices_.data(), stagingBuffer->getSize());
    graphicsDevice_->unmap(stagingBuffer);

    shared_ptr<IndexBuffer> indexBuffer = graphicsDevice_->createIndexBuffer(indices_.size(), VK_INDEX_TYPE_UINT32);
    scene_->setIndexBuffer(indexBuffer);

    graphicsDevice_->bind(indexBuffer);

    VkCommandPool graphicsCommandPool = graphicsDevice_->getGraphicsCommandPool();
    shared_ptr<CommandBuffer> commandBuffer = graphicsDevice_->createCommandBuffer(graphicsCommandPool);
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    commandBuffer->copyBuffer(stagingBuffer, indexBuffer);
    commandBuffer->end();

    graphicsDevice_->getGraphicsQueue()->flush(commandBuffer);

    graphicsDevice_->destroyCommandBuffer(commandBuffer, graphicsCommandPool);
}

// ---------------------------------------------------------------------------------------------------------------------
// #####################################################################################################################
// ---------------------------------------------------------------------------------------------------------------------

ModelLoader::ModelLoader(shared_ptr<GraphicsDevice> graphicsDevice)
    : pimpl_(new ModelLoaderImpl(move(graphicsDevice)),
        [](ModelLoaderImpl* pimpl) { delete pimpl; }) {}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Model>& ModelLoader::load(const path& scenePath)
{
    return pimpl_->load(scenePath);
}

// ---------------------------------------------------------------------------------------------------------------------
