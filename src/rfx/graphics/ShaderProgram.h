#pragma once

#include "rfx/graphics/VertexShader.h"
#include "rfx/graphics/FragmentShader.h"


namespace rfx {

class ShaderProgram
{
public:
    ShaderProgram(
        VertexShaderPtr vertexShader,
        FragmentShaderPtr fragmentShader);

    [[nodiscard]] const VertexShaderPtr& getVertexShader() const;
    [[nodiscard]] const FragmentShaderPtr& getFragmentShader() const;

private:
    VertexShaderPtr vertexShader;
    FragmentShaderPtr fragmentShader;
};

using ShaderProgramPtr = std::shared_ptr<ShaderProgram>;

} // namespace rfx
