#pragma once

#include "TestApplication.h"
#include "TexturedMultiLightShader.h"
#include "rfx/scene/Scene.h"


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

    void updateSceneData() override;

private:
    void loadScene();
    void buildRenderGraph() override;

    ScenePtr scene;
    PointLightPtr pointLight;
    SpotLightPtr spotLight;
    std::shared_ptr<TexturedMultiLightShader> shader;
};

} // namespace rfx


