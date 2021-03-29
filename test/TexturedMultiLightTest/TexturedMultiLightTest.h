#pragma once

#include "TestApplication.h"
#include "TexturedMultiLightShader.h"


namespace rfx {

class TexturedMultiLightTest : public TestApplication
{
public:
    TexturedMultiLightTest();

protected:
    void initGraphics() override;
    void initShaderFactory(MaterialShaderFactory& shaderFactory) override;
    void createMeshResources() override;
    void updateShaderData() override;
    void cleanup() override;

    void setViewMatrix(const glm::mat4 &viewMatrix) override;

private:
    void loadScene();
    void buildRenderGraph();

    ModelPtr scene;
    PointLightPtr pointLight;
    SpotLightPtr spotLight;
    std::shared_ptr<TexturedMultiLightShader> shader;
};

} // namespace rfx


