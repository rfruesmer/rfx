#pragma once

#include "test/TestApplication.h"

namespace rfx
{

class TexturedCubeTest : public TestApplication
{
public:
    explicit TexturedCubeTest(handle_t instanceHandle);

protected:
    void initPipelineLayout() override;
    void initDescriptorPool();
    void initDescriptorSet() override;
    void loadTexture();

private:
    std::unique_ptr<Texture2D> texture;
};

} // namespace rfx

