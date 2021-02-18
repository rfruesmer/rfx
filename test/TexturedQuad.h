#pragma once

#include "ColoredQuad.h"

namespace rfx::test {

class TexturedQuad : public ColoredQuad
{
protected:
    void buildScene() override;
    void createDescriptorPool() override;
    void createDescriptorSetLayout() override;
    void createDescriptorSets() override;
    void createVertexBuffer() override;
    void cleanup() override;

private:
    void createTexture();

    std::shared_ptr<Texture2D> texture;
};

} // namespace rfx
