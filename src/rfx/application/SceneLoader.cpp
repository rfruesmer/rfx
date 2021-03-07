#include "rfx/pch.h"
#include "rfx/application/SceneLoader.h"
#include "rfx/scene/PointLight.h"

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

class SceneLoader::SceneLoaderImpl
{
public:
    explicit SceneLoaderImpl(shared_ptr<GraphicsDevice>&& graphicsDevice)
        : graphicsDevice_(move(graphicsDevice)) {}

    const shared_ptr<Scene>& load(const path& scenePath, const VertexFormat& vertexFormat);
    void clear();
    void loadImages();
    static vector<byte> convertToRGBA(const tinygltf::Image& gltfImage);
    void loadTextures();
    void loadMaterials();
    void loadMaterial(size_t index);
    void loadMeshes();
    void prepareGeometryBuffers();
    void loadMesh(const tinygltf::Mesh& gltfMesh);
    void loadVertices(const tinygltf::Primitive& glTFPrimitive);
    const float* getBufferData(const tinygltf::Primitive& glTFPrimitive, const string& attribute);
    void appendVertexData(
        uint32_t vertexCount,
        const float* positionBuffer,
        const float* normalsBuffer,
        const float* texCoordsBuffer,
        const float* tangentsBuffer);
    uint32_t loadIndices(const tinygltf::Primitive& glTFPrimitive, uint32_t vertexStart);
    void loadLights();
    void loadLight(const tinygltf::Value::Object& gltfLight);
    shared_ptr<Light> loadPointLight(const tinygltf::Value::Object& gltfLight);
    void loadNodes();
    void loadNode(
        const tinygltf::Node& node,
        const shared_ptr<SceneNode>& parentNode);
    static mat4 getLocalTransformOf(const tinygltf::Node& gltfNode) ;

    void buildVertexBuffer();
    void buildIndexBuffer();

    shared_ptr<GraphicsDevice> graphicsDevice_;
    tinygltf::Model gltfModel_;
    VertexFormat vertexFormat_;
    shared_ptr<Scene> scene_;
    uint32_t vertexCount_ = 0;
    vector<float> vertexData_;
    vector<uint32_t> indices_;
};

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Scene>& SceneLoader::SceneLoaderImpl::load(
    const path& scenePath,
    const VertexFormat& vertexFormat)
{
    RFX_CHECK_STATE(exists(scenePath), "File not found: " + scenePath.string());

    clear();

    vertexFormat_ = vertexFormat;

    tinygltf::TinyGLTF gltfContext;
    string error;
    string warning;

    bool result = gltfContext.LoadASCIIFromFile(&gltfModel_, &error, &warning, scenePath.string());
    RFX_CHECK_STATE(result,
        "Failed to load glTF file: " + scenePath.string() + "\n"
        + "Errors: " + error
        + "Warnings: " + warning);

    loadImages();
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

void SceneLoader::SceneLoaderImpl::clear()
{
    gltfModel_ = {};

    scene_.reset();
    scene_ = make_shared<Scene>();
    vertexCount_ = 0;
    vertexData_.clear();
    indices_.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::loadImages()
{
    for (const auto& gltfImage : gltfModel_.images) {
        vector<byte> imageData;

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

        scene_->addTexture(graphicsDevice_->createTexture2D(gltfImage.name, imageDesc, imageData, false));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

vector<byte> SceneLoader::SceneLoaderImpl::convertToRGBA(const tinygltf::Image& gltfImage)
{
    const unsigned char* rgbData = gltfImage.image.data();

    size_t rgbaDataSize = gltfImage.width * gltfImage.height * 4;
    vector<byte> rgbaData(rgbaDataSize);

    size_t rgbIndex = 0;
    size_t rgbaIndex = 0;

    for (size_t i = 0; i < gltfImage.width * gltfImage.height; ++i) {
        rgbaData[rgbaIndex + 0] = static_cast<byte>(rgbData[rgbIndex + 0]);
        rgbaData[rgbaIndex + 1] = static_cast<byte>(rgbData[rgbIndex + 1]);
        rgbaData[rgbaIndex + 2] = static_cast<byte>(rgbData[rgbIndex + 2]);
        rgbaData[rgbaIndex + 3] = static_cast<byte>(255);

        rgbIndex += 3;
        rgbaIndex += 4;
    }

    return rgbaData;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::loadTextures()
{
    for (size_t i = 0; i < gltfModel_.textures.size(); i++) {
        RFX_CHECK_STATE(gltfModel_.textures[i].source == i, "Indexed images not implemented yet");
    }

    RFX_CHECK_STATE(gltfModel_.textures.size() == scene_->getTextures().size(), "Indexed images not implemented yet");
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::loadMaterials()
{
    for (size_t i = 0; i < gltfModel_.materials.size(); i++) {
        loadMaterial(i);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::loadMaterial(size_t index)
{
    const tinygltf::Material& glTFMaterial = gltfModel_.materials[index];

    const auto material = make_shared<Material>();

    if (const auto& it = glTFMaterial.values.find("baseColorFactor");
            it != glTFMaterial.values.end()) {
        material->setBaseColorFactor(make_vec4(it->second.ColorFactor().data()));
    }

    if (const auto& it = glTFMaterial.values.find("baseColorTexture");
            it != glTFMaterial.values.end()) {
        const shared_ptr<Texture2D>& baseColorTexture = scene_->getTexture(it->second.TextureIndex());
        material->setBaseColorTexture(baseColorTexture);
    }

    if (glTFMaterial.normalTexture.index > -1) {
        const shared_ptr<Texture2D>& normalTexture = scene_->getTexture(glTFMaterial.normalTexture.index);
        material->setNormalTexture(normalTexture);
    }

    scene_->addMaterial(material);
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::loadMeshes()
{
    prepareGeometryBuffers();

    for (const auto& gltfMesh : gltfModel_.meshes) {
        loadMesh(gltfMesh);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::prepareGeometryBuffers()
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

void SceneLoader::SceneLoaderImpl::loadMesh(const tinygltf::Mesh& gltfMesh)
{
    auto mesh = make_unique<Mesh>();

    for (size_t i = 0; i < gltfMesh.primitives.size(); ++i) {
        const tinygltf::Primitive& glTFPrimitive = gltfMesh.primitives[i];

        auto firstIndex = static_cast<uint32_t>(indices_.size());
        auto vertexStart = vertexCount_;

        loadVertices(glTFPrimitive);
        uint32_t indexCount = loadIndices(glTFPrimitive, vertexStart);

        mesh->addSubMesh({
            .firstIndex = firstIndex,
            .indexCount = indexCount,
            .materialIndex = static_cast<uint32_t>(glTFPrimitive.material)
        });
    }

    scene_->addMesh(move(mesh));
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::loadVertices(const tinygltf::Primitive& glTFPrimitive)
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

    // TEXCOORD_0
    const float* texCoordsBuffer = nullptr;
    if (vertexFormat_.containsTexCoords()) {
        texCoordsBuffer = getBufferData(glTFPrimitive, "TEXCOORD_0");
        RFX_CHECK_STATE(texCoordsBuffer != nullptr, "Texture coordinates are missing in input file!");
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
        texCoordsBuffer,
        tangentsBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

const float* SceneLoader::SceneLoaderImpl::getBufferData(
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

void SceneLoader::SceneLoaderImpl::appendVertexData(
    uint32_t vertexCount,
    const float* positionBuffer,
    const float* normalsBuffer,
    const float* texCoordsBuffer,
    const float* tangentsBuffer)
{
    uint32_t destIndex = vertexCount_ * (vertexFormat_.getVertexSize() / sizeof(float));
    vec3 normal;

    for (uint32_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {

        memcpy(&vertexData_[destIndex], &positionBuffer[vertexIndex * 3], sizeof(vec3));
        destIndex += 3;

        if (normalsBuffer) {
            normal = normalize(make_vec3(&normalsBuffer[vertexIndex * 3]));
            memcpy(&vertexData_[destIndex], &normal, sizeof(vec3));
            destIndex += 3;
        }

        if (texCoordsBuffer) {
            memcpy(&vertexData_[destIndex], &texCoordsBuffer[vertexIndex * 2], sizeof(vec2));
            destIndex += 2;
        }

        if (tangentsBuffer) {
            memcpy(&vertexData_[destIndex], &tangentsBuffer[vertexIndex * 4], sizeof(vec4));
            destIndex += 4;
        }
    }

    vertexCount_ += vertexCount;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t SceneLoader::SceneLoaderImpl::loadIndices(
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

void SceneLoader::SceneLoaderImpl::loadLights()
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

void SceneLoader::SceneLoaderImpl::loadLight(const tinygltf::Value::Object& gltfLight)
{
    shared_ptr<Light> light;

    const string type = gltfLight.find("type")->second.Get<string>();
    if (type == GLTF_POINT_LIGHT_TYPE) {
        light = loadPointLight(gltfLight);
    }
    else  {
        RFX_THROW_NOT_IMPLEMENTED();
    }

    scene_->addLight(light);
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Light> SceneLoader::SceneLoaderImpl::loadPointLight(const tinygltf::Value::Object& gltfLight)
{
    string id;
    if (gltfLight.contains("name")) {
        id = gltfLight.find("name")->second.Get<string>();
    }

    vec3 color { 1.0f };
    if (gltfLight.contains("color")) {
        const auto& gltfColor = gltfLight.find("color")->second.Get<tinygltf::Value::Array>();
        color = vec3(
            gltfColor[0].GetNumberAsDouble(),
            gltfColor[1].GetNumberAsDouble(),
            gltfColor[2].GetNumberAsDouble()
        );
    }

    if (gltfLight.contains("range")) {
        RFX_THROW_NOT_IMPLEMENTED();
    }

    if (gltfLight.contains("intensity")) {
        // TODO: consider intensity
    }

    auto pointLight = make_shared<PointLight>(id);
    pointLight->setColor(color);

    return pointLight;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::loadNodes()
{
    RFX_CHECK_STATE(gltfModel_.scenes.size() == 1, "Multiple scenes not supported yet");

    const tinygltf::Scene& gltfScene = gltfModel_.scenes[0];
    for (const auto nodeIndex : gltfScene.nodes) {
        const tinygltf::Node& node = gltfModel_.nodes[nodeIndex];
        loadNode(node, scene_->getRootNode());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::loadNode(
    const tinygltf::Node& gltfNode,
    const shared_ptr<SceneNode>& parentNode)
{
    auto node = make_shared<SceneNode>(parentNode);
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

    for (size_t i = 0; i < gltfNode.children.size(); ++i) {
        loadNode(gltfModel_.nodes[gltfNode.children[i]], node);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

mat4 SceneLoader::SceneLoaderImpl::getLocalTransformOf(const tinygltf::Node& gltfNode)
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

void SceneLoader::SceneLoaderImpl::buildVertexBuffer()
{
    const size_t vertexBufferSize = vertexData_.size() * sizeof(float);

    shared_ptr<Buffer> stagingBuffer = graphicsDevice_->createBuffer(
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* mappedMemory = nullptr;
    graphicsDevice_->bind(stagingBuffer);
    graphicsDevice_->map(stagingBuffer, &mappedMemory);
    memcpy(mappedMemory, vertexData_.data(), stagingBuffer->getSize());
    graphicsDevice_->unmap(stagingBuffer);

    shared_ptr<VertexBuffer> vertexBuffer = graphicsDevice_->createVertexBuffer(vertexCount_, vertexFormat_);
    scene_->setVertexBuffer(vertexBuffer);


    graphicsDevice_->bind(vertexBuffer);

    VkCommandPool graphicsCommandPool = graphicsDevice_->getGraphicsCommandPool();
    shared_ptr<CommandBuffer> commandBuffer = graphicsDevice_->createCommandBuffer(graphicsCommandPool);
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    commandBuffer->copyBuffer(stagingBuffer, vertexBuffer);
    commandBuffer->end();

    graphicsDevice_->getGraphicsQueue()->flush(commandBuffer);

    graphicsDevice_->destroyCommandBuffer(commandBuffer, graphicsCommandPool);
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::buildIndexBuffer()
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

SceneLoader::SceneLoader(shared_ptr<GraphicsDevice> graphicsDevice)
    : pimpl_(new SceneLoaderImpl(move(graphicsDevice)),
        [](SceneLoaderImpl* pimpl) { delete pimpl; }) {}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Scene>& SceneLoader::load(
    const path& scenePath,
    const VertexFormat& vertexFormat)
{
    return pimpl_->load(scenePath, vertexFormat);
}

// ---------------------------------------------------------------------------------------------------------------------
