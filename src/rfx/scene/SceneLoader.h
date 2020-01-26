#pragma once

#include "rfx/scene/Scene.h"
#include "rfx/scene/ModelDefinitionDeserializer.h"
#include "rfx/core/JsonDeserializer.h"

namespace rfx
{

class SceneLoader : public JsonDeserializer
{
public:
    explicit SceneLoader(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        VkRenderPass renderPass,
        const std::unordered_map<std::string, EffectDefinition>& effectDefaults);

    std::unique_ptr<Scene> load(const Json::Value& jsonScene);

    const std::vector<std::shared_ptr<Effect>>& getEffects() const;

private:
    void loadCamera() const;
    void loadLights();
    void loadModels();
    ModelDefinition deserialize(const Json::Value& jsonModelDefinition,
        const ModelDefinitionDeserializer& deserializer) const;
    std::shared_ptr<Effect> loadEffect(const EffectDefinition& effectDefinition);
    std::shared_ptr<Effect> createEffect(const EffectDefinition& effectDefinition,
        std::unique_ptr<ShaderProgram>& shaderProgram,
        const std::vector<std::shared_ptr<Texture2D>>& textures);
    std::shared_ptr<Mesh> loadModel(const ModelDefinition& modelDefinition,
        const std::shared_ptr<Effect>& effect) const;
    void attachToSceneGraph(const std::shared_ptr<Mesh>& mesh, 
        const ModelDefinition& modelDefinition) const;

    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::unordered_map<std::string, EffectDefinition> effectDefaults;

    Json::Value jsonScene;
    std::unique_ptr<Scene> scene;
    std::vector<std::shared_ptr<Effect>> effects;
    VkRenderPass renderPass;
};

}
