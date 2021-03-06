#include "rfx/pch.h"
#include "rfx/application/SceneLoader.h"
#include "rfx/scene/Material.h"
#include "rfx/scene/Vertex.h"
#include "rfx/common/Logger.h"

#include <nlohmann/json.hpp>
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>


using namespace rfx;
using namespace glm;
using namespace std;
using namespace filesystem;


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
    void loadNodes();
    void loadNode(
        const tinygltf::Node& node,
        const mat4& parentNodeWorldTransform);
    static mat4 getLocalTransformOf(const tinygltf::Node& gltfNode) ;
    unique_ptr<Mesh> loadMesh(const tinygltf::Mesh& gltfMesh, const mat4& worldTransform);
    void loadVertices(const tinygltf::Primitive& glTFPrimitive);
    uint32_t loadIndices(const tinygltf::Primitive& glTFPrimitive, uint32_t vertexStart);

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
    loadNodes();
    buildVertexBuffer();
    buildIndexBuffer();

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

        scene_->addTexture(graphicsDevice_->createTexture2D(imageDesc, imageData, false));
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

    scene_->addMaterial(material);
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::loadNodes()
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

    const tinygltf::Scene& gltfScene = gltfModel_.scenes[0];
    for (size_t i = 0; i < gltfScene.nodes.size(); ++i) {
        const tinygltf::Node& node = gltfModel_.nodes[gltfScene.nodes[i]];
        loadNode(node, mat4 { 1.0f });
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::loadNode(
    const tinygltf::Node& gltfNode,
    const mat4& parentNodeWorldTransform)
{
    const mat4 localTransform = getLocalTransformOf(gltfNode);
    const mat4 worldTransform = parentNodeWorldTransform * localTransform;

    for (size_t i = 0; i < gltfNode.children.size(); ++i) {
        loadNode(gltfModel_.nodes[gltfNode.children[i]], worldTransform);
    }

    if (gltfNode.mesh > -1) {
        scene_->addMesh(move(loadMesh(gltfModel_.meshes[gltfNode.mesh], worldTransform)));
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

unique_ptr<Mesh> SceneLoader::SceneLoaderImpl::loadMesh(
    const tinygltf::Mesh& gltfMesh,
    const mat4& worldTransform)
{
    auto mesh = make_unique<Mesh>();
    mesh->setWorldTransform(worldTransform);


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

    return mesh;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::loadVertices(const tinygltf::Primitive& glTFPrimitive)
{
    const float* positionBuffer = nullptr;
    const float* normalsBuffer = nullptr;
    const float* texCoordsBuffer = nullptr;
    uint32_t vertexCount = 0;

    // Get buffer data for vertex normals
    if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end()) {
        const tinygltf::Accessor& accessor = gltfModel_.accessors[glTFPrimitive.attributes.find("POSITION")->second];
        const tinygltf::BufferView& view = gltfModel_.bufferViews[accessor.bufferView];
        positionBuffer = reinterpret_cast<const float*>(&(gltfModel_.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
        vertexCount = accessor.count;
    }
    // Get buffer data for vertex normals
    if (vertexFormat_.containsNormals() && glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end()) {
        const tinygltf::Accessor& accessor = gltfModel_.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
        const tinygltf::BufferView& view = gltfModel_.bufferViews[accessor.bufferView];
        normalsBuffer = reinterpret_cast<const float*>(&(gltfModel_.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
    }
    // Get buffer data for vertex texture coordinates
    // glTF supports multiple sets, we only load the first one
    if (vertexFormat_.containsTexCoords() && glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end()) {
        const tinygltf::Accessor& accessor = gltfModel_.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
        const tinygltf::BufferView& view = gltfModel_.bufferViews[accessor.bufferView];
        texCoordsBuffer = reinterpret_cast<const float*>(&(gltfModel_.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
    }

    // Append data to model's vertex buffer
    uint32_t floatCount = vertexCount_ * (vertexFormat_.getVertexSize() / sizeof(float));
    vec3 normal;

    for (uint32_t i = 0; i < vertexCount; i++) {

        memcpy(&vertexData_[floatCount], &positionBuffer[i * 3], 3 * sizeof(float));
        floatCount += 3;

        if (vertexFormat_.containsNormals()) {
            if (normalsBuffer) {
                normal = normalize(make_vec3(&normalsBuffer[i * 3]));
                memcpy(&vertexData_[floatCount], &normal, 3 * sizeof(float));
            }
            else {
                RFX_THROW_NOT_IMPLEMENTED(); // generation of normals not implemented yet
            }
            floatCount += 3;
        }

        if (vertexFormat_.containsTexCoords()) {
            RFX_CHECK_STATE(texCoordsBuffer != nullptr, "Texture coordinates are missing in input file");

            memcpy(&vertexData_[floatCount], &texCoordsBuffer[i * 2], 2 * sizeof(float));
            floatCount += 2;
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
