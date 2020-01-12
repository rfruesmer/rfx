#pragma once

#include "test/TestApplication.h"
#include "rfx/scene/Mesh.h"

namespace rfx
{

class CubeTest : public TestApplication
{
public:
    explicit CubeTest(handle_t instanceHandle);

    void initialize() override;

protected:
    void initScene() override;
    void loadModel();
    void loadShaders();
    void initDescriptorPool();
    void initPipelineLayout() override;
    void initPipeline() override;
    void initDescriptorSet() override;
    void initCommandBuffers() override;

private:
    std::shared_ptr<Mesh> cube;
};

} // namespace rfx

