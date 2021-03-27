#pragma once

#include "rfx/scene/Material.h"
#include "rfx/graphics/GraphicsDevice.h"
#include <rfx/graphics/VertexShader.h>
#include <rfx/graphics/FragmentShader.h>


namespace rfx {

class MaterialShader
{
public:
    virtual ~MaterialShader() = default;

    virtual void loadShaders(const std::shared_ptr<Material>& material, const std::filesystem::path& shadersDirectory);
    virtual void update(const std::shared_ptr<Material>& material) const = 0;

    [[nodiscard]] const std::shared_ptr<VertexShader>& getVertexShader() const;
    [[nodiscard]] const std::shared_ptr<FragmentShader>& getFragmentShader() const;

protected:
    explicit MaterialShader(std::shared_ptr<GraphicsDevice> graphicsDevice);

    [[nodiscard]] virtual std::vector<std::string> buildShaderDefines(const std::shared_ptr<Material>& material, const VertexFormat& vertexFormat);
    [[nodiscard]] virtual std::vector<std::string> buildVertexShaderInputs(const VertexFormat& vertexFormat);
    [[nodiscard]] virtual std::vector<std::string> buildVertexShaderOutputs(const VertexFormat& vertexFormat);
    [[nodiscard]] virtual std::vector<std::string> buildFragmentShaderInputs(const VertexFormat& vertexFormat);


    std::shared_ptr<GraphicsDevice> graphicsDevice_;
    std::shared_ptr<VertexShader> vertexShader;
    std::shared_ptr<FragmentShader> fragmentShader;
};

} // namespace rfx