#pragma once

#include "rfx/scene/SceneNode.h"
#include "rfx/graphics/effect/Light.h"

namespace rfx
{

class Scene
{
public:
    Scene();


    void add(std::unique_ptr<SceneNode>& node) const;
    [[nodiscard]] const std::unique_ptr<SceneNode>& getRootNode() const;

    void add(const std::shared_ptr<Light>& light);
    [[nodiscard]] const std::vector<std::shared_ptr<Light>>& getLights() const;

private:
    std::unique_ptr<SceneNode> rootNode;
    std::vector<std::shared_ptr<Light>> lights;
};

} // namespace rfx
