#pragma once

#include "test/TestApplication.h"
#include "test/texture-mapping/Texture2DEffect.h"

namespace rfx
{

class TexturedQuadTest : public TestApplication
{
public:
    explicit TexturedQuadTest(handle_t instanceHandle);

    void initialize() override;

protected:
    void initEffects();
    void initScene() override;
    [[nodiscard]] const std::shared_ptr<Camera>& getCamera() const override;
    void initCommandBuffers() override;

private:
    void createQuadMesh();
    void initCamera();

    std::shared_ptr<Camera> camera;
    std::shared_ptr<Mesh> quadMesh;
    std::shared_ptr<Texture2DEffect> textureEffect;
};

}
