#pragma once

#include "test/CubeTest.h"
#include "rfx/scene/Mesh.h"

namespace rfx
{

class TexturedCubesTest : public CubeTest
{
public:
    explicit TexturedCubesTest(handle_t instanceHandle);

    void initialize() override;

protected:
    void initScene() override;
    void loadModels() override;
    void loadTexture();
    void initDescriptorSetLayout() override;
    void initDescriptorPool() override;
    void initDescriptorSet() override;

private:
    std::unique_ptr<Texture2D> texture;
};

} // namespace rfx

