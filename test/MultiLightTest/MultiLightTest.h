#pragma once

#include "TestApplication.h"
#include "MultiLightShader.h"


namespace rfx {

class MultiLightTest : public TestApplication
{
public:
    MultiLightTest();

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
    std::shared_ptr<MultiLightShader> shader;
};

} // namespace rfx


