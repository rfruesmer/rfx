#pragma once

#include "test/TestApplication.h"

namespace rfx
{

class TexturedCubeTest : public TestApplication
{
public:
    explicit TexturedCubeTest(HINSTANCE instanceHandle);

    uint32_t getVertexSize() const override;
    uint32_t getVertexCount() const override;
    const std::byte* getVertexData() const override;

    std::string getVertexShaderPath() const override;
    std::string getFragmentShaderPath() const override;

protected:
    void initPipelineLayout() override;
    void initDescriptorPool() override;
    void initDescriptorSet() override;
    void loadTexture();
    void initVertexBuffer() override;

private:
    std::unique_ptr<Texture2D> texture;
};

} // namespace rfx

