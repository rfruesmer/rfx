#pragma once

#include "TestMaterialShader.h"

namespace rfx {

class SampleViewerShader : public TestMaterialShader
{
public:
    struct MaterialData {
        glm::vec4 baseColor { 0.0f };
    };

    static const std::string ID;

    explicit SampleViewerShader(const GraphicsDevicePtr& graphicsDevice);

    [[nodiscard]] std::vector<std::byte> createDataFor(const MaterialPtr& material) const override;
    [[nodiscard]] const void* getData() const override;
    [[nodiscard]] uint32_t getDataSize() const override;

    std::vector<std::string> getShaderDefinesFor(const MaterialPtr& material) override;
    std::vector<std::string> getVertexShaderInputsFor(const MaterialPtr& material) override;
    std::vector<std::string> getVertexShaderOutputsFor(const MaterialPtr& material) override;
    std::vector<std::string> getFragmentShaderInputsFor(const MaterialPtr& material) override;

private:
    struct ShaderData {
        glm::mat4 pad0;
    };

    ShaderData data {};
};

} // namespace rfx