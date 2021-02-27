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
        : graphicsDevice(move(graphicsDevice)) {}

    const shared_ptr<Scene>& load(const path& scenePath);
    void clear();
    void loadImages();
    static vector<byte> convertToRGBA(const tinygltf::Image& gltfImage);
    void loadTextures();
    void loadMaterials();
    void loadMaterial(size_t index);
    void loadNodes();
    void loadNode(
        const tinygltf::Node& node,
        const shared_ptr<SceneNode>& parent);
    static mat4 getLocalTransformFor(const tinygltf::Node& gltfNode) ;
    unique_ptr<Mesh> loadMesh(const tinygltf::Mesh& gltfMesh);
    void loadVertices(const tinygltf::Primitive& glTFPrimitive);
    uint32_t loadIndices(const tinygltf::Primitive& glTFPrimitive, uint32_t vertexStart);

    void buildVertexBuffer();
    void buildIndexBuffer();

    shared_ptr<GraphicsDevice> graphicsDevice;
    tinygltf::Model gltfModel;
    shared_ptr<Scene> scene;
    vector<Vertex> vertices;
    vector<uint32_t> indices;
};

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Scene>& SceneLoader::SceneLoaderImpl::load(const path& scenePath)
{
    clear();

    tinygltf::TinyGLTF gltfContext;
    string error;
    string warning;

    bool result = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, scenePath.string());
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

    return scene;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::clear()
{
    gltfModel = {};

    scene.reset();
    scene = make_shared<Scene>();
    vertices.clear();
    indices.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::loadImages()
{
    for (const auto& gltfImage : gltfModel.images) {
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

        scene->addTexture(graphicsDevice->createTexture2D(imageDesc, imageData, false));
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
    for (size_t i = 0; i < gltfModel.textures.size(); i++) {
        RFX_CHECK_STATE(gltfModel.textures[i].source == i, "Indexed images not implemented yet");
    }

    RFX_CHECK_STATE(gltfModel.textures.size() == scene->getTextures().size(), "Indexed images not implemented yet");
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::loadMaterials()
{
    for (size_t i = 0; i < gltfModel.materials.size(); i++) {
        loadMaterial(i);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::loadMaterial(size_t index)
{
    const tinygltf::Material& glTFMaterial = gltfModel.materials[index];

    const auto material = make_shared<Material>();

    if (const auto& it = glTFMaterial.values.find("baseColorFactor");
            it != glTFMaterial.values.end()) {
        material->setBaseColorFactor(make_vec4(it->second.ColorFactor().data()));
    }

    if (const auto& it = glTFMaterial.values.find("baseColorTexture");
            it != glTFMaterial.values.end()) {
        const shared_ptr<Texture2D>& baseColorTexture = scene->getTexture(it->second.TextureIndex());
        material->setBaseColorTexture(baseColorTexture);
    }

    scene->addMaterial(material);
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::loadNodes()
{
    const tinygltf::Scene& gltfScene = gltfModel.scenes[0];
    for (size_t i = 0; i < gltfScene.nodes.size(); i++) {
        const tinygltf::Node& node = gltfModel.nodes[gltfScene.nodes[i]];
        loadNode(node, nullptr);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::loadNode(
    const tinygltf::Node& gltfNode,
    const shared_ptr<SceneNode>& parentNode)
{
    auto sceneNode = make_shared<SceneNode>(parentNode);

    mat4 localTransform = getLocalTransformFor(gltfNode);
    sceneNode->setLocalTransform(localTransform);

    for (size_t i = 0; i < gltfNode.children.size(); ++i) {
        loadNode(gltfModel.nodes[gltfNode.children[i]], sceneNode);
    }

    if (gltfNode.mesh > -1) {
        sceneNode->addMesh(move(loadMesh(gltfModel.meshes[gltfNode.mesh])));
    }

    if (parentNode) {
        parentNode->addChild(sceneNode);
    }
    else {
        RFX_CHECK_STATE(scene->getRootNode() == nullptr, "");
        scene->setRootNode(sceneNode);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

mat4 SceneLoader::SceneLoaderImpl::getLocalTransformFor(const tinygltf::Node& gltfNode)
{
    mat4 localTransform {1.0f };

    if (gltfNode.translation.size() == 3) {
        localTransform = translate(localTransform, vec3(make_vec3(gltfNode.translation.data())));
    }
    if (gltfNode.rotation.size() == 4) {
        quat q = make_quat(gltfNode.rotation.data());
        localTransform *= mat4(q);
    }
    if (gltfNode.scale.size() == 3) {
        localTransform = scale(localTransform, vec3(make_vec3(gltfNode.scale.data())));
    }
    if (gltfNode.matrix.size() == 16) {
        localTransform = make_mat4x4(gltfNode.matrix.data());
    }
    return localTransform;
}

// ---------------------------------------------------------------------------------------------------------------------

unique_ptr<Mesh> SceneLoader::SceneLoaderImpl::loadMesh(const tinygltf::Mesh& gltfMesh)
{
    auto mesh = make_unique<Mesh>();

    for (size_t i = 0; i < gltfMesh.primitives.size(); ++i) {
        const tinygltf::Primitive& glTFPrimitive = gltfMesh.primitives[i];

        auto firstIndex = static_cast<uint32_t>(indices.size());
        auto vertexStart = static_cast<uint32_t>(vertices.size());

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
        const tinygltf::Accessor& accessor = gltfModel.accessors[glTFPrimitive.attributes.find("POSITION")->second];
        const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];
        positionBuffer = reinterpret_cast<const float*>(&(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
        vertexCount = accessor.count;
    }
    // Get buffer data for vertex normals
    if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end()) {
        const tinygltf::Accessor& accessor = gltfModel.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
        const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];
        normalsBuffer = reinterpret_cast<const float*>(&(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
    }
    // Get buffer data for vertex texture coordinates
    // glTF supports multiple sets, we only load the first one
    if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end()) {
        const tinygltf::Accessor& accessor = gltfModel.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
        const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];
        texCoordsBuffer = reinterpret_cast<const float*>(&(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
    }

    // Append data to model's vertex buffer
    for (size_t v = 0; v < vertexCount; v++) {
        Vertex vertex {
            .pos = vec4(make_vec3(&positionBuffer[v * 3]), 1.0f),
            .color = vec3(1.0f),
            .normal = normalize(vec3(normalsBuffer ? make_vec3(&normalsBuffer[v * 3]) : vec3(0.0f))),
            .uv = texCoordsBuffer ? make_vec2(&texCoordsBuffer[v * 2]) : vec3(0.0f)
        };
        vertices.push_back(vertex);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t SceneLoader::SceneLoaderImpl::loadIndices(
    const tinygltf::Primitive& glTFPrimitive,
    uint32_t vertexStart)
{
    const tinygltf::Accessor& accessor = gltfModel.accessors[glTFPrimitive.indices];
    const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
    const tinygltf::Buffer& buffer = gltfModel.buffers[bufferView.buffer];

    auto indexCount = static_cast<uint32_t>(accessor.count);

    // glTF supports different component types of indices
    switch (accessor.componentType) {
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
            vector<uint32_t> buf(accessor.count);
            memcpy(&buf[0], &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
            for (size_t index = 0; index < accessor.count; index++) {
                indices.push_back(buf[index] + vertexStart);
            }
            break;
        }
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
            vector<uint16_t> buf(accessor.count);
            memcpy(&buf[0], &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
            for (size_t index = 0; index < accessor.count; index++) {
                indices.push_back(buf[index] + vertexStart);
            }
            break;
        }
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
            vector<uint8_t> buf(accessor.count);
            memcpy(&buf[0], &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
            for (size_t index = 0; index < accessor.count; index++) {
                indices.push_back(buf[index] + vertexStart);
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
    VertexFormat vertexFormat(
        VertexFormat::COORDINATES
        | VertexFormat::COLORS_3
        | VertexFormat::NORMALS
        | VertexFormat::TEXCOORDS);

    const size_t vertexBufferSize = vertices.size() * vertexFormat.getVertexSize();

    shared_ptr<Buffer> stagingBuffer = graphicsDevice->createBuffer(
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* mappedMemory = nullptr;
    graphicsDevice->bind(stagingBuffer);
    graphicsDevice->map(stagingBuffer, &mappedMemory);
    memcpy(mappedMemory, vertices.data(), stagingBuffer->getSize());
    graphicsDevice->unmap(stagingBuffer);

    shared_ptr<VertexBuffer> vertexBuffer = graphicsDevice->createVertexBuffer(vertices.size(), vertexFormat);
    scene->setVertexBuffer(vertexBuffer);


    graphicsDevice->bind(vertexBuffer);

    VkCommandPool graphicsCommandPool = graphicsDevice->getGraphicsCommandPool();
    shared_ptr<CommandBuffer> commandBuffer = graphicsDevice->createCommandBuffer(graphicsCommandPool);
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    commandBuffer->copyBuffer(stagingBuffer, vertexBuffer);
    commandBuffer->end();

    graphicsDevice->getGraphicsQueue()->flush(commandBuffer);

    graphicsDevice->destroyCommandBuffer(commandBuffer, graphicsCommandPool);
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::SceneLoaderImpl::buildIndexBuffer()
{
    const VkDeviceSize bufferSize = indices.size() * sizeof(uint32_t);
    shared_ptr<Buffer> stagingBuffer = graphicsDevice->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* mappedMemory = nullptr;
    graphicsDevice->bind(stagingBuffer);
    graphicsDevice->map(stagingBuffer, &mappedMemory);
    memcpy(mappedMemory, indices.data(), stagingBuffer->getSize());
    graphicsDevice->unmap(stagingBuffer);

    shared_ptr<IndexBuffer> indexBuffer = graphicsDevice->createIndexBuffer(indices.size(), VK_INDEX_TYPE_UINT32);
    scene->setIndexBuffer(indexBuffer);

    graphicsDevice->bind(indexBuffer);

    VkCommandPool graphicsCommandPool = graphicsDevice->getGraphicsCommandPool();
    shared_ptr<CommandBuffer> commandBuffer = graphicsDevice->createCommandBuffer(graphicsCommandPool);
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    commandBuffer->copyBuffer(stagingBuffer, indexBuffer);
    commandBuffer->end();

    graphicsDevice->getGraphicsQueue()->flush(commandBuffer);

    graphicsDevice->destroyCommandBuffer(commandBuffer, graphicsCommandPool);
}

// ---------------------------------------------------------------------------------------------------------------------
// #####################################################################################################################
// ---------------------------------------------------------------------------------------------------------------------

SceneLoader::SceneLoader(shared_ptr<GraphicsDevice> graphicsDevice)
    : pimpl_(new SceneLoaderImpl(move(graphicsDevice)),
        [](SceneLoaderImpl* pimpl) { delete pimpl; }) {}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Scene>& SceneLoader::load(const path& scenePath)
{
    return pimpl_->load(scenePath);
}

// ---------------------------------------------------------------------------------------------------------------------
