#pragma once

#include "rfx/scene/Material.h"
#include "rfx/graphics/GraphicsDevice.h"
#include <rfx/graphics/VertexShader.h>
#include <rfx/graphics/FragmentShader.h>


namespace rfx {

class MaterialShader
{
public:
    virtual void loadShaders(const std::shared_ptr<Material>& material, const std::filesystem::path& shadersDirectory);
    virtual void update(const std::shared_ptr<Material>& material) const = 0;

    [[nodiscard]] const std::string& getId() const;

    [[nodiscard]] const std::string& getVertexShaderId() const;
    [[nodiscard]] const std::shared_ptr<VertexShader>& getVertexShader() const;

    [[nodiscard]] const std::string& getFragmentShaderId() const;
    [[nodiscard]] const std::shared_ptr<FragmentShader>& getFragmentShader() const;

    [[nodiscard]] virtual std::vector<std::string> getShaderDefinesFor(const MaterialPtr& material);
    [[nodiscard]] virtual std::vector<std::string> getVertexShaderInputsFor(const MaterialPtr& material);
    [[nodiscard]] virtual std::vector<std::string> getVertexShaderOutputsFor(const MaterialPtr& material);
    [[nodiscard]] virtual std::vector<std::string> getFragmentShaderInputsFor(const MaterialPtr& material);

protected:
    MaterialShader(
        GraphicsDevicePtr graphicsDevice,
        std::string id,
        std::string vertexShaderId,
        std::string fragmentShaderId);
    virtual ~MaterialShader();


    std::shared_ptr<GraphicsDevice> graphicsDevice_;

    std::string id;
    std::string vertexShaderId;
    std::shared_ptr<VertexShader> vertexShader;
    std::string fragmentShaderId;
    std::shared_ptr<FragmentShader> fragmentShader;
};

using MaterialShaderPtr = std::shared_ptr<MaterialShader>;

} // namespace rfx