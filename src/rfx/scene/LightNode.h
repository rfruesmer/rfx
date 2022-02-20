#pragma once

#include "rfx/scene/Node.h"
#include "rfx/scene/Light.h"


namespace rfx {

class LightNode : public Node
{
public:
    explicit LightNode(const NodePtr& parent);

    void addLight(LightPtr light);

private:
    void update() override;

    std::vector<LightPtr> lights_;
};

using LightNodePtr = std::shared_ptr<LightNode>;

} // namespace rfx
