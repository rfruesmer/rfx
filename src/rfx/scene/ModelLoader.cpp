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
    static const int assimpFlags = 
          aiProcess_FlipWindingOrder
        | aiProcess_Triangulate
        | aiProcess_PreTransformVertices;


    Assimp::Importer assimpImporter;
    const aiScene* assimpScene = assimpImporter.ReadFile(modelPath.string().c_str(), assimpFlags);

    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
    for (uint32_t i = 0; i < assimpScene->mNumMeshes; i++) {
        vertexCount += assimpScene->mMeshes[i]->mNumVertices;
        indexCount += assimpScene->mMeshes[i]->mNumFaces * 3;
    }

    uint32_t vertexBufferSize = vertexCount * vertexFormat.getVertexSize();
    vector<float> vertexData(vertexBufferSize);
    uint32_t vertexDataIndex = 0;
    vector<uint32_t> indexData(indexCount);
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

    const shared_ptr<Buffer> stagingVertexBuffer = graphicsDevice->createBuffer(
        vertexBufferSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    const shared_ptr<VertexBuffer> vertexBuffer = 
        graphicsDevice->createVertexBuffer(vertexCount, vertexFormat);

    const uint32_t indexBufferSize = indexCount * sizeof(uint32_t);

    const shared_ptr<Buffer> stagingIndexBuffer = graphicsDevice->createBuffer(
        indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    const shared_ptr<IndexBuffer> indexBuffer = 
        graphicsDevice->createIndexBuffer(indexCount, VK_INDEX_TYPE_UINT32);

    const shared_ptr<CommandPool>& commandPool = graphicsDevice->getTempCommandPool();
    const shared_ptr<CommandBuffer> commandBuffer = commandPool->allocateCommandBuffer();
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    commandBuffer->copyBuffer(stagingVertexBuffer, vertexBuffer);
    commandBuffer->copyBuffer(stagingIndexBuffer, indexBuffer);
    commandBuffer->end();

    const shared_ptr<Queue>& queue = graphicsDevice->getGraphicsQueue();
    queue->submit(commandBuffer, nullptr);
    queue->waitIdle();

    commandPool->freeCommandBuffer(commandBuffer);

    return make_shared<Mesh>(graphicsDevice, vertexBuffer, indexBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------
