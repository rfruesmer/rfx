#include "rfx/pch.h"
#include "rfx/scene/ModelLoader.h"
#include "rfx/graphics/VertexFormat.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

ModelLoader::ModelLoader(const shared_ptr<GraphicsDevice>& graphicsDevice)
    : graphicsDevice(graphicsDevice) {}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Mesh> ModelLoader::load(const filesystem::path& modelPath, const VertexFormat& vertexFormat)
{
    loadInternal(modelPath, vertexFormat);
    createBuffers(vertexFormat);
    copyBuffers();

    return make_shared<Mesh>(graphicsDevice, vertexBuffer, indexBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::loadInternal(const std::filesystem::path& modelPath, const VertexFormat& vertexFormat)
{
    static const int assimpFlags =
        aiProcess_FlipWindingOrder
        | aiProcess_Triangulate
        | aiProcess_PreTransformVertices;


    Assimp::Importer assimpImporter;
    const aiScene* assimpScene = assimpImporter.ReadFile(modelPath.string().c_str(), assimpFlags);

    vertexCount = 0;
    indexCount = 0;
    for (uint32_t i = 0; i < assimpScene->mNumMeshes; i++) {
        vertexCount += assimpScene->mMeshes[i]->mNumVertices;
        indexCount += assimpScene->mMeshes[i]->mNumFaces * 3;
    }

    vertexBufferSize = vertexCount * vertexFormat.getVertexSize();
    vertexData.resize(vertexBufferSize);
    indexData.resize(indexCount);

    uint32_t vertexDataIndex = 0;
    uint32_t indexDataIndex = 0;

    for (uint32_t i = 0; i < assimpScene->mNumMeshes; ++i) {
        const auto& assimpMesh = assimpScene->mMeshes[i];

        for (uint32_t j = 0; j < assimpScene->mMeshes[i]->mNumVertices; j++) {
            vertexData[vertexDataIndex++] = assimpMesh->mVertices[j].x;
            vertexData[vertexDataIndex++] = -1.0f * assimpMesh->mVertices[j].y;
            vertexData[vertexDataIndex++] = assimpMesh->mVertices[j].z;

            if (vertexFormat.containsColors()) {
                if (assimpMesh->HasVertexColors(0)) {
                    vertexData[vertexDataIndex++] = assimpMesh->mColors[0][j].r;
                    vertexData[vertexDataIndex++] = assimpMesh->mColors[0][j].g;
                    vertexData[vertexDataIndex++] = assimpMesh->mColors[0][j].b;
                }
                else {
                    vertexData[vertexDataIndex++] = 1.0f;
                    vertexData[vertexDataIndex++] = 1.0f;
                    vertexData[vertexDataIndex++] = 1.0f;
                }
            }

            if (vertexFormat.containsNormals()) {
                vertexData[vertexDataIndex++] = assimpMesh->mNormals[j].x;
                vertexData[vertexDataIndex++] = assimpMesh->mNormals[j].y;
                vertexData[vertexDataIndex++] = assimpMesh->mNormals[j].z;
            }

            if (vertexFormat.containsTexCoords()) {
                vertexData[vertexDataIndex++] = assimpMesh->mTextureCoords[0][j].x;
                vertexData[vertexDataIndex++] = assimpMesh->mTextureCoords[0][j].y;
            }
        }

        const uint32_t indexBase = indexDataIndex;

        for (uint32_t k = 0; k < assimpMesh->mNumFaces; ++k) {
            indexData[indexDataIndex++] = indexBase + assimpMesh->mFaces[k].mIndices[0];
            indexData[indexDataIndex++] = indexBase + assimpMesh->mFaces[k].mIndices[1];
            indexData[indexDataIndex++] = indexBase + assimpMesh->mFaces[k].mIndices[2];
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::createBuffers(const VertexFormat& vertexFormat)
{
    stagingVertexBuffer = graphicsDevice->createBuffer(
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingVertexBuffer->load(vertexBufferSize,
        reinterpret_cast<const std::byte*>(vertexData.data()));
    stagingVertexBuffer->bind();

    vertexBuffer = graphicsDevice->createVertexBuffer(vertexCount, vertexFormat);
    vertexBuffer->bind();

    const uint32_t indexBufferSize = indexCount * sizeof(uint32_t);

    stagingIndexBuffer = graphicsDevice->createBuffer(
        indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingIndexBuffer->load(indexBufferSize,
        reinterpret_cast<const std::byte*>(indexData.data()));
    stagingIndexBuffer->bind();

    indexBuffer = graphicsDevice->createIndexBuffer(indexCount, VK_INDEX_TYPE_UINT32);
    indexBuffer->bind();
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelLoader::copyBuffers()
{
    const shared_ptr<CommandPool>& commandPool = graphicsDevice->getTempCommandPool();
    const shared_ptr<CommandBuffer> commandBuffer = commandPool->allocateCommandBuffer();

    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    commandBuffer->copyBuffer(stagingVertexBuffer, vertexBuffer);
    commandBuffer->copyBuffer(stagingIndexBuffer, indexBuffer);
    commandBuffer->end();

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    VkFence fence = graphicsDevice->createFence(fenceCreateInfo);

    const shared_ptr<Queue>& queue = graphicsDevice->getGraphicsQueue();
    queue->submit(commandBuffer, fence);

    const VkResult result = graphicsDevice->waitForFences(1, &fence, true, DEFAULT_FENCE_TIMEOUT);
    RFX_CHECK_STATE(result == VK_SUCCESS, "failed to submit copy commands");

    graphicsDevice->destroyFence(fence);

    commandPool->freeCommandBuffer(commandBuffer);

    vertexCount = 0;
    vertexBufferSize = 0;
    vertexData.clear();
    stagingVertexBuffer.reset();

    indexCount = 0;
    indexData.clear();
    stagingIndexBuffer.reset();
}

// ---------------------------------------------------------------------------------------------------------------------
