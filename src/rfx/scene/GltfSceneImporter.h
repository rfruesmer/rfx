#include "rfx/scene/SceneImporter.h"
#include "rfx/scene/PointLight.h"
#include "rfx/scene/SpotLight.h"
#include "rfx/scene/LightNode.h"
#include "rfx/common/Algorithm.h"

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

struct ModelData
{
    VertexFormat vertexFormat;
    uint32_t vertexCount = 0;
    vector<float> vertexData;
    vector<uint32_t> indices;

    vector<uint32_t> gltfMeshIndices;
    unordered_map<uint32_t, uint32_t> gltfToModelMeshMap;
};

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

class GltfSceneImporter : public SceneImporter
{
public:
    explicit GltfSceneImporter(
        GraphicsDevicePtr graphicsDevice)
            : graphicsDevice_(move(graphicsDevice)) {}

    ScenePtr import(const path& scenePath) override;

private:
    void clear(const string& sceneId);
    VertexFormat getVertexFormatFrom(const tinygltf::Primitive& primitive);
    void checkCompatibility();

    void loadImages();
    static vector<std::byte> convertToRGBA(const tinygltf::Image& gltfImage);
    void loadSamplers();
    void loadTextures();
    void loadTexture(const tinygltf::Texture& gltfTexture);

    void loadModels();
    void buildModelLookupTable();
    void loadModel();

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
        const float* colorsBuffer,
        const float* normalsBuffer,
        const float** texCoordsBuffers,
        const float* tangentsBuffer);
    uint32_t appendCoordinates(const float* positionBuffer, uint32_t vertexIndex, uint32_t destIndex);
    uint32_t appendColors(const float* colorsBuffer, uint32_t vertexIndex, uint32_t destIndex);
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
    void loadLightNodes();
    void loadLightNode(
        const tinygltf::Node& gltfNode,
        const LightNodePtr& parentNode);

    void loadModelNodes();
    void loadModelNode(
        const tinygltf::Node& node,
        const shared_ptr<ModelNode>& parentNode);
    static mat4 getLocalTransformOf(const tinygltf::Node& gltfNode) ;

    void buildVertexBuffer();
    void buildIndexBuffer();

    shared_ptr<GraphicsDevice> graphicsDevice_;
    tinygltf::Model gltfModel_;

    vector<SamplerDesc> samplers_;
    vector<ImagePtr> images_;
    vector<Texture2DPtr> textures_;
    
    ScenePtr scene_;
    ModelPtr currentModel;
    ModelData currentModelData;
    vector<ModelData> modelData;
};

// ---------------------------------------------------------------------------------------------------------------------

ScenePtr GltfSceneImporter::import(const path& scenePath)
{
    RFX_CHECK_STATE(exists(scenePath), "File not found: " + scenePath.string());

    const string sceneId = scenePath.stem().string();
    clear(sceneId);


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

    checkCompatibility();

    loadImages();
    loadSamplers();
    loadTextures();
    loadLights();
    loadLightNodes();
    loadModels();

    scene_->compile();

    return scene_;
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::clear(const string& sceneId)
{
    gltfModel_ = {};

    samplers_.clear();
    images_.clear();

    scene_ = make_shared<Scene>(sceneId);

    currentModel.reset();
    currentModelData = {};
    modelData.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

VertexFormat GltfSceneImporter::getVertexFormatFrom(const tinygltf::Primitive& primitive)
{
    unsigned int formatMask = 0;
    unsigned int texCoordSetCount = 0;

    if (primitive.attributes.contains("POSITION")) {
        formatMask |= VertexFormat::COORDINATES;
    }

    if (primitive.attributes.contains("COLOR_0")) {
        const tinygltf::Accessor& accessor =
            gltfModel_.accessors[primitive.attributes.find("COLOR_0")->second];
        formatMask |= accessor.type == TINYGLTF_TYPE_VEC3
            ? VertexFormat::COLORS_3
            : VertexFormat::COLORS_4;
    }

    if (primitive.attributes.contains("NORMAL")) {
        formatMask |= VertexFormat::NORMALS;
    }

    if (primitive.attributes.contains("TEXCOORD_0")) {
        formatMask |= VertexFormat::TEXCOORDS;
        texCoordSetCount = 1;
    }

    if (primitive.attributes.contains("TEXCOORD_1")) {
        texCoordSetCount++;
    }

    if (primitive.attributes.contains("TEXCOORD_2")) {
        texCoordSetCount++;
    }

    if (primitive.attributes.contains("TEXCOORD_3")) {
        texCoordSetCount++;
    }

    if (primitive.attributes.contains("TEXCOORD_4")) {
        texCoordSetCount++;
    }

    if (primitive.attributes.contains("TANGENT")) {
        formatMask |= VertexFormat::TANGENTS;
    }

    return { formatMask, texCoordSetCount };
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::checkCompatibility()
{
    RFX_CHECK_STATE(gltfModel_.scenes.size() == 1, "Multiple scenes not supported yet");

    for (const auto& mesh : gltfModel_.meshes)
    {
        if (mesh.primitives.empty()) {
            continue;
        }

        // check that each primitive of a single mesh is using the same vertex format:
        const VertexFormat referenceVertexFormat = getVertexFormatFrom(mesh.primitives[0]);

        for (const auto& primitive : mesh.primitives) {
            RFX_CHECK_STATE(getVertexFormatFrom(primitive) == referenceVertexFormat,
                "different vertex formats per mesh aren't supported");
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::loadImages()
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

vector<std::byte> GltfSceneImporter::convertToRGBA(const tinygltf::Image& gltfImage)
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

void GltfSceneImporter::loadSamplers()
{
    ranges::transform(gltfModel_.samplers, back_inserter(samplers_), toSamplerDesc);
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::loadTextures()
{
    for (const auto& texture : gltfModel_.textures) {
        loadTexture(texture);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::loadTexture(const tinygltf::Texture& gltfTexture)
{
    const shared_ptr<Image>& image = images_[gltfTexture.source];
    const ImageDesc imageDesc = image->getDesc();
    VkImageView imageView = graphicsDevice_->createImageView(
        image,
        imageDesc.format,
        VK_IMAGE_ASPECT_COLOR_BIT,
        imageDesc.mipLevels);
    const SamplerDesc& samplerDesc = samplers_[gltfTexture.sampler];

    const Texture2DPtr& texture = graphicsDevice_->createTexture2D(image, imageView, samplerDesc);
    textures_.push_back(texture);
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::loadModels()
{
    buildModelLookupTable();

    for (const auto& modelData : modelData)
    {
        currentModelData = modelData;

        loadModel();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::buildModelLookupTable()
{
    unordered_map<VertexFormat, vector<uint32_t>> vertexFormatToMeshMap;

    for (size_t i = 0; i < gltfModel_.meshes.size(); ++i)
    {
        const auto& mesh = gltfModel_.meshes[i];
        if (mesh.primitives.empty()) {
            continue;
        }

        VertexFormat vertexFormat = getVertexFormatFrom(mesh.primitives[0]);
        vertexFormatToMeshMap[vertexFormat].push_back(i);
    }


    for (const auto& [vertexFormat, gltfMeshIndices] : vertexFormatToMeshMap)
    {
        unordered_map<uint32_t, uint32_t> gltfToModelMeshMap;

        uint32_t meshIndex = 0;
        for (uint32 gltfMeshIndex : gltfMeshIndices) {
            gltfToModelMeshMap[gltfMeshIndex] = meshIndex;
            meshIndex++;
        }

        modelData.push_back({
            .vertexFormat = VertexFormat(vertexFormat),
            .gltfMeshIndices = gltfMeshIndices,
            .gltfToModelMeshMap = gltfToModelMeshMap
        });
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::loadModel()
{
    const string modelId = scene_->getId() + "-#" + to_string(scene_->getModelCount());

    currentModel = make_shared<Model>(modelId);

    loadMaterials();
    loadMeshes();
    loadModelNodes();
    buildVertexBuffer();
    buildIndexBuffer();

    scene_->add(currentModel);
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::loadMaterials()
{
    for (const auto& gltfMaterial : gltfModel_.materials) {
        loadMaterial(gltfMaterial);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::loadMaterial(const tinygltf::Material& glTFMaterial) const
{
    tinygltf::Value::Object extras = glTFMaterial.extras.Get<tinygltf::Value::Object>();
    const string shaderId = extras.contains("shader")
            ? extras.at("shader").Get<string>()
            : "";

    const auto material = make_shared<Material>(
        glTFMaterial.name,
        currentModelData.vertexFormat,
        shaderId);

    // TODO: specular-glossiness material model
    const tinygltf::PbrMetallicRoughness& gltfMetallicRoughness = glTFMaterial.pbrMetallicRoughness;

    material->setBaseColorFactor(make_vec4(gltfMetallicRoughness.baseColorFactor.data()));
    if (gltfMetallicRoughness.baseColorTexture.index > -1) {
        material->setBaseColorTexture(
            textures_.at(gltfMetallicRoughness.baseColorTexture.index),
            gltfMetallicRoughness.baseColorTexture.texCoord);
    }

    if (gltfMetallicRoughness.metallicRoughnessTexture.index > -1) {
        material->setMetallicRoughnessTexture(
            textures_.at(gltfMetallicRoughness.metallicRoughnessTexture.index),
            gltfMetallicRoughness.metallicRoughnessTexture.texCoord);
    }

    material->setMetallicFactor(static_cast<float>(gltfMetallicRoughness.metallicFactor));
    material->setRoughnessFactor(static_cast<float>(gltfMetallicRoughness.roughnessFactor));

    if (glTFMaterial.normalTexture.index > -1) {
        material->setNormalTexture(
            textures_.at(glTFMaterial.normalTexture.index),
            glTFMaterial.normalTexture.texCoord);
    }

    if (glTFMaterial.occlusionTexture.index > -1) {
        material->setOcclusionTexture(
            textures_.at(glTFMaterial.occlusionTexture.index),
            glTFMaterial.occlusionTexture.texCoord,
            static_cast<float>(glTFMaterial.occlusionTexture.strength));
    }

    if (glTFMaterial.emissiveTexture.index > -1) {
        material->setEmissiveFactor(make_vec3(glTFMaterial.emissiveFactor.data()));
        material->setEmissiveTexture(
            textures_.at(glTFMaterial.emissiveTexture.index),
            glTFMaterial.emissiveTexture.texCoord);
    }

    currentModel->addMaterial(material);
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::loadMeshes()
{
    prepareGeometryBuffers();

    for (uint32_t index : currentModelData.gltfMeshIndices) {
        loadMesh(gltfModel_.meshes[index]);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::prepareGeometryBuffers()
{
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;

    for (uint32_t index : currentModelData.gltfMeshIndices)
    {
        const auto& gltfMesh = gltfModel_.meshes[index];

        for (const auto& gltfPrimitive : gltfMesh.primitives)
        {
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

    currentModelData.vertexData.resize(vertexCount * (currentModelData.vertexFormat.getVertexSize() / sizeof(float)));
    currentModelData.indices.reserve(indexCount);
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::loadMesh(const tinygltf::Mesh& gltfMesh)
{
    auto mesh = make_unique<Mesh>();

    for (const tinygltf::Primitive& glTFPrimitive : gltfMesh.primitives) {

        auto firstIndex = static_cast<uint32_t>(currentModelData.indices.size());
        auto vertexStart = currentModelData.vertexCount;

        loadVertices(glTFPrimitive);
        uint32_t indexCount = loadIndices(glTFPrimitive, vertexStart);

        mesh->addSubMesh({
            firstIndex,
            indexCount,
            currentModel->getMaterial(glTFPrimitive.material)
        });
    }

    currentModel->addMesh(move(mesh));
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::loadVertices(const tinygltf::Primitive& glTFPrimitive)
{
    const tinygltf::Accessor& accessor = gltfModel_.accessors[glTFPrimitive.attributes.find("POSITION")->second];
    const uint32_t vertexCount = accessor.count;

    // POSITION
    const float* positionBuffer = getBufferData(glTFPrimitive, "POSITION");

    // COLOR_0
    const float* colorsBuffer = nullptr;
    if (currentModelData.vertexFormat.containsColors3() || currentModelData.vertexFormat.containsColors4()) {
        colorsBuffer = getBufferData(glTFPrimitive, "COLOR_0");
    }

    // NORMAL
    const float* normalsBuffer = nullptr;
    if (currentModelData.vertexFormat.containsNormals()) {
        normalsBuffer = getBufferData(glTFPrimitive, "NORMAL");
        RFX_CHECK_STATE(normalsBuffer != nullptr, "Tangents generation not implemented yet!");
    }

    // TEXCOORD
    uint32_t inputTexCoordSetCount = 0;
    uint32_t outputTexCoordSetCount = currentModelData.vertexFormat.getTexCoordSetCount();
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
    if (currentModelData.vertexFormat.containsTangents()) {
        tangentsBuffer = getBufferData(glTFPrimitive, "TANGENT");
        RFX_CHECK_STATE(tangentsBuffer != nullptr, "Tangents generation not implemented yet!");
    }

    appendVertexData(
        vertexCount,
        positionBuffer,
        colorsBuffer,
        normalsBuffer,
        texCoordsBuffers,
        tangentsBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

const float* GltfSceneImporter::getBufferData(
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

void GltfSceneImporter::appendVertexData(
    uint32_t vertexCount,
    const float* positionBuffer,
    const float* colorsBuffer,
    const float* normalsBuffer,
    const float** texCoordsBuffers,
    const float* tangentsBuffer)
{
    uint32_t destIndex = currentModelData.vertexCount * (currentModelData.vertexFormat.getVertexSize() / sizeof(float));

    for (uint32_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
        destIndex += appendCoordinates(positionBuffer, vertexIndex, destIndex);
        destIndex += appendColors(colorsBuffer, vertexIndex, destIndex);
        destIndex += appendNormals(normalsBuffer, vertexIndex, destIndex);
        destIndex += appendTexCoords(texCoordsBuffers, vertexIndex, destIndex);
        destIndex += appendTangents(tangentsBuffer, vertexIndex, destIndex);
    }

    currentModelData.vertexCount += vertexCount;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t GltfSceneImporter::appendCoordinates(
    const float* positionBuffer,
    uint32_t vertexIndex,
    uint32_t destIndex)
{
    memcpy(&currentModelData.vertexData[destIndex], &positionBuffer[vertexIndex * 3], sizeof(vec3));

    return 3;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t GltfSceneImporter::appendColors(
    const float* colorsBuffer,
    uint32_t vertexIndex,
    uint32_t destIndex)
{
    if (!colorsBuffer) {
        return 0;
    }

    if (currentModelData.vertexFormat.containsColors3()) {
        memcpy(&currentModelData.vertexData[destIndex], &colorsBuffer[vertexIndex * 3], sizeof(vec3));
        return 3;
    }
    else if (currentModelData.vertexFormat.containsColors4()){
        memcpy(&currentModelData.vertexData[destIndex], &colorsBuffer[vertexIndex * 4], sizeof(vec4));
        return 4;
    }

    RFX_CHECK_STATE(false, "invalid vertex format");

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t GltfSceneImporter::appendNormals(
    const float* normalsBuffer,
    uint32_t vertexIndex,
    uint32_t destIndex)
{
    if (normalsBuffer) {
        vec3 normal = normalize(make_vec3(&normalsBuffer[vertexIndex * 3]));
        memcpy(&currentModelData.vertexData[destIndex], &normal, sizeof(vec3));

        return 3;
    }

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t GltfSceneImporter::appendTexCoords(
    const float** texCoordsBuffers,
    const uint32_t vertexIndex,
    const uint32_t destIndex)
{
    uint32_t offset = 0;

    for (uint32_t i = 0; i < VertexFormat::MAX_TEXCOORDSET_COUNT; ++i) {
        if (texCoordsBuffers[i] == nullptr) {
            break;
        }

        memcpy(&currentModelData.vertexData[destIndex + offset], &texCoordsBuffers[i][vertexIndex * 2], sizeof(vec2));
        offset += 2;
    }

    return offset;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t GltfSceneImporter::appendTangents(
    const float* tangentsBuffer,
    uint32_t vertexIndex,
    uint32_t destIndex)
{
    if (tangentsBuffer) {
        memcpy(&currentModelData.vertexData[destIndex], &tangentsBuffer[vertexIndex * 4], sizeof(vec4));
        return 4;
    }

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t GltfSceneImporter::loadIndices(
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
                currentModelData.indices.push_back(buf[index] + vertexStart);
            }
            break;
        }
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
            vector<uint16_t> buf(accessor.count);
            memcpy(&buf[0], &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
            for (size_t index = 0; index < accessor.count; index++) {
                currentModelData.indices.push_back(buf[index] + vertexStart);
            }
            break;
        }
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
            vector<uint8_t> buf(accessor.count);
            memcpy(&buf[0], &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
            for (size_t index = 0; index < accessor.count; index++) {
                currentModelData.indices.push_back(buf[index] + vertexStart);
            }
            break;
        }
        default:
            RFX_THROW("Index component type "s + to_string(accessor.componentType) + " not supported!"s);
    }

    return indexCount;
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::loadLights()
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

void GltfSceneImporter::loadLight(const tinygltf::Value::Object& gltfLight)
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

shared_ptr<Light> GltfSceneImporter::loadPointLight(const tinygltf::Value::Object& gltfLight)
{
    const GLTFLightProperties lightProperties = getLightProperties(gltfLight);

    auto pointLight = make_shared<PointLight>(lightProperties.name);
    pointLight->setColor(lightProperties.color);
    // TODO: consider intensity


    return pointLight;
}

// ---------------------------------------------------------------------------------------------------------------------

GLTFLightProperties GltfSceneImporter::getLightProperties(const tinygltf::Value::Object& gltfLight)
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

shared_ptr<Light> GltfSceneImporter::loadSpotLight(const tinygltf::Value::Object& gltfLight)
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

void GltfSceneImporter::loadLightNodes()
{
    const tinygltf::Scene& gltfScene = gltfModel_.scenes[0];
    for (const auto nodeIndex : gltfScene.nodes) {
        const tinygltf::Node& node = gltfModel_.nodes[nodeIndex];
        loadLightNode(node, scene_->getLightsRootNode());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::loadLightNode(
    const tinygltf::Node& gltfNode,
    const LightNodePtr& parentNode)
{
    auto node = make_shared<LightNode>(parentNode);
    node->setLocalTransform(getLocalTransformOf(gltfNode));

    if (gltfNode.extensions.contains("KHR_lights_punctual")) {
        const auto& lightExtension = gltfNode.extensions.find("KHR_lights_punctual")->second.Get<tinygltf::Value::Object>();
        int lightIndex = lightExtension.find("light")->second.GetNumberAsInt();
        node->addLight(scene_->getLight(lightIndex));
    }

    parentNode->addChild(node);

    for (int childIndex : gltfNode.children) {
        loadLightNode(gltfModel_.nodes[childIndex], node);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::loadModelNodes()
{
    const tinygltf::Scene& gltfScene = gltfModel_.scenes[0];

    for (const auto nodeIndex : gltfScene.nodes) {
        const tinygltf::Node& node = gltfModel_.nodes[nodeIndex];
        loadModelNode(node, currentModel->getRootNode());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GltfSceneImporter::loadModelNode(
    const tinygltf::Node& gltfNode,
    const shared_ptr<ModelNode>& parentNode)
{
    auto node = make_shared<ModelNode>(parentNode);
    node->setLocalTransform(getLocalTransformOf(gltfNode));

    if (contains(currentModelData.gltfMeshIndices, gltfNode.mesh))
    {
        const auto it = currentModelData.gltfToModelMeshMap.find(gltfNode.mesh);
        RFX_CHECK_STATE(it != currentModelData.gltfToModelMeshMap.end(), "Invalid mesh mapping");

        const uint32_t meshIndex = it->second;
        node->addMesh(currentModel->getMesh(meshIndex));
    }

    parentNode->addChild(node);

    for (int childIndex : gltfNode.children) {
        loadModelNode(gltfModel_.nodes[childIndex], node);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

mat4 GltfSceneImporter::getLocalTransformOf(const tinygltf::Node& gltfNode)
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

void GltfSceneImporter::buildVertexBuffer()
{
    const size_t vertexDataSize = currentModelData.vertexData.size() * sizeof(float);

    shared_ptr<VertexBuffer> vertexBuffer = graphicsDevice_->createVertexBuffer(
        currentModelData.vertexCount,
        currentModelData.vertexFormat);
    currentModel->setVertexBuffer(vertexBuffer);
    graphicsDevice_->bind(vertexBuffer);

    shared_ptr<Buffer> stagingBuffer = graphicsDevice_->createBuffer(
        vertexDataSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* mappedMemory = nullptr;
    graphicsDevice_->bind(stagingBuffer);
    graphicsDevice_->map(stagingBuffer, &mappedMemory);
    memcpy(mappedMemory, currentModelData.vertexData.data(), vertexDataSize);
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

void GltfSceneImporter::buildIndexBuffer()
{
    const VkDeviceSize bufferSize = currentModelData.indices.size() * sizeof(uint32_t);
    shared_ptr<Buffer> stagingBuffer = graphicsDevice_->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* mappedMemory = nullptr;
    graphicsDevice_->bind(stagingBuffer);
    graphicsDevice_->map(stagingBuffer, &mappedMemory);
    memcpy(mappedMemory, currentModelData.indices.data(), stagingBuffer->getSize());
    graphicsDevice_->unmap(stagingBuffer);

    shared_ptr<IndexBuffer> indexBuffer = graphicsDevice_->createIndexBuffer(currentModelData.indices.size(), VK_INDEX_TYPE_UINT32);
    currentModel->setIndexBuffer(indexBuffer);

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
