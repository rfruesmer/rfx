#pragma once

#include "TestApplication.h"
#include "TexturedPBRShader.h"
#include "rfx/scene/Scene.h"


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
    void buildRenderGraph() override;

    ScenePtr scene;
    PointLightPtr pointLight;
    std::shared_ptr<TexturedPBRShader> shader;
};

} // namespace rfx


