#pragma once

#include "test/TestApplication.h"
#include "rfx/graphics/Texture2DEffect.h"

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
    void initCommandBuffers() override;

private:
    void createQuadMesh();

    std::shared_ptr<Mesh> quadMesh;
    std::shared_ptr<Texture2DEffect> textureEffect;
};

}
