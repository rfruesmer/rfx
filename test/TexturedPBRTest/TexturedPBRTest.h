#pragma once

#include "TestApplication.h"
#include "TexturedPBRShader.h"


namespace rfx {

class TexturedPBRTest : public TestApplication
{
protected:
    void initGraphics() override;
    void initShaderFactory(MaterialShaderFactory& shaderFactory) override;
    void createMeshResources() override;
    void updateShaderData() override;
    void updateDevTools() override;
    void cleanup() override;

private:
    void loadScene();
    void buildRenderGraph();

    ModelPtr scene;
    PointLightPtr pointLight;
    SpotLightPtr spotLight;
    std::shared_ptr<TexturedPBRShader> shader;
};

} // namespace rfx


