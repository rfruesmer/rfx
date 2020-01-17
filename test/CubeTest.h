#pragma once

#include "test/TestApplication.h"
#include "rfx/scene/SceneNode.h"
#include "rfx/scene/Mesh.h"

namespace rfx
{
class ModelDefinition;

class CubeTest : public TestApplication
{
public:
    CubeTest(handle_t instanceHandle);

    void initialize() override;

protected:
    CubeTest(const std::filesystem::path& configurationPath, handle_t instanceHandle);

    void initScene() override;
    void createSceneGraphRootNode();
    virtual void loadModels();
    std::shared_ptr<Mesh> loadModel(const ModelDefinition& modelDefinition) const;
    virtual void loadShaders(const ModelDefinition& modelDefinition, const std::shared_ptr<Mesh>& mesh) const;
    void attachToSceneGraph(const std::shared_ptr<Mesh>& mesh) const;
    virtual void initDescriptorPool();
    void initPipeline() override;
    void initDescriptorSet() override;
    void initCommandBuffers() override;

    std::unique_ptr<SceneNode> sceneGraph;
};

} // namespace rfx

