#pragma once

#include "test/CubeTest.h"
#include "rfx/scene/Mesh.h"

namespace rfx
{

class TexturedCubesTest : public CubeTest
{
public:
    explicit TexturedCubesTest(handle_t instanceHandle);

    void initialize() override;

protected:
    void loadModels() override;
    void loadTexture(const Json::Value& jsonModel, const std::shared_ptr<Mesh>& mesh) const;
    void initDescriptorSetLayout() override;
    void initDescriptorPool() override;
    void initDescriptorSet() override;
};

} // namespace rfx

