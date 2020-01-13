#pragma once

#include "test/CubeTest.h"
#include "rfx/scene/Mesh.h"

namespace rfx
{

class TexturedCubeTest : public CubeTest
{
public:
    explicit TexturedCubeTest(handle_t instanceHandle);

    void initialize() override;

protected:
    void initScene() override;
    void loadModel();
    void loadShaders();
    void loadTexture();
    void initDescriptorSetLayout() override;
    void initDescriptorPool();
    void initDescriptorSet() override;

private:
    std::unique_ptr<Texture2D> texture;
};

} // namespace rfx

