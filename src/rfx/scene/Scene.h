#pragma once

#include "rfx/scene/Model.h"
#include "rfx/scene/LightNode.h"


namespace rfx {

class Scene
{
public:
    explicit Scene(std::string id);

    [[nodiscard]] const std::string& getId() const;

    void compile();

    void add(ModelPtr model);
    [[nodiscard]] const ModelPtr& getModel(size_t index);
    [[nodiscard]] const std::vector<ModelPtr>& getModels() const;
    [[nodiscard]] size_t getModelCount() const;

    void addLight(const LightPtr& light);
    [[nodiscard]] const LightPtr& getLight(size_t index) const;
    [[nodiscard]] const std::vector<LightPtr>& getLights() const;
    [[nodiscard]] uint32_t getLightCount() const;
    [[nodiscard]] const LightNodePtr& getLightsRootNode() const;

private:
    std::string id;
    std::vector<ModelPtr> models;
    std::vector<LightPtr> lights_;
    LightNodePtr lightsRootNode;
};

using ScenePtr = std::shared_ptr<Scene>;

} // namespace rfx
