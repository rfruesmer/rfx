#pragma once

#include "test/TestApplication.h"
#include "rfx/scene/Mesh.h"

namespace rfx
{

class CubeTest : public TestApplication
{
public:
    CubeTest(handle_t instanceHandle);

    void initialize() override;

protected:
    CubeTest(std::filesystem::path configurationPath, handle_t instanceHandle);

    void initScene();
    void loadModel();
    void loadShaders();
    void initDescriptorPool();
    void initPipeline() override;
    void initDescriptorSet() override;
    void initCommandBuffers() override;

    std::shared_ptr<Mesh> cube;
};

} // namespace rfx

