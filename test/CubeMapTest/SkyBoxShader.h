#pragma once

#include "TestMaterialShader.h"


namespace rfx {

class SkyBoxShader : public TestMaterialShader
{
public:
    static const std::string ID;

    explicit SkyBoxShader(GraphicsDevicePtr& graphicsDevice);

    [[nodiscard]] std::vector<std::byte> createDataFor(const MaterialPtr& material) const override;

    [[nodiscard]] const void* getData() const override;
    [[nodiscard]] uint32_t getDataSize() const override;

private:
};

} // namespace rfx
