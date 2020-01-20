#pragma once

#include "rfx/scene/SceneNode.h"

namespace rfx
{

class Scene
{
public:
    Scene();

    void add(std::unique_ptr<SceneNode>& node) const;

    const std::unique_ptr<SceneNode>& getRootNode() const;

private:
    std::unique_ptr<SceneNode> rootNode;
};

} // namespace rfx
