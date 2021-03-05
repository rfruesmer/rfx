#pragma once

#include "TestEffect.h"
#include "rfx/scene/Scene.h"


namespace rfx {

class FragmentPhongEffect : public TestEffect
{
public:
    static inline const VertexFormat VERTEX_FORMAT {
        VertexFormat::COORDINATES | VertexFormat::NORMALS
    };

    FragmentPhongEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<Scene>& scene);

    [[nodiscard]] VertexFormat getVertexFormat() const override { return VERTEX_FORMAT; };
    [[nodiscard]] std::string getVertexShaderFileName() const override;
    [[nodiscard]] std::string getFragmentShaderFileName() const override;
};

} // namespace rfx