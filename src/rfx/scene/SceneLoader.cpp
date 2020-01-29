#include "rfx/pch.h"
#include "rfx/scene/SceneLoader.h"
#include "rfx/application/Texture2DLoader.h"
#include "rfx/application/ShaderLoader.h"
#include "rfx/scene/ModelLoader.h"
#include "rfx/scene/ModelDefinition.h"
#include "rfx/graphics/effect/LightDefinitionDeserializer.h"
#include "rfx/graphics/effect/VertexColorEffect.h"
#include "rfx/graphics/effect/Texture2DEffect.h"
#include "rfx/graphics/effect/DirectionalLightEffect.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

SceneLoader::SceneLoader(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass,
    const std::unordered_map<std::string, EffectDefinition>& effectDefaults)
        : graphicsDevice(graphicsDevice),
          renderPass(renderPass),
          effectDefaults(effectDefaults) {}

// ---------------------------------------------------------------------------------------------------------------------

unique_ptr<Scene> SceneLoader::load(const Json::Value& jsonScene)
{
    this->jsonScene = jsonScene;
    scene = make_unique<Scene>();

    loadCamera();
    loadLights();
    loadModels();

    return move(scene);
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::loadCamera() const
{
    const auto camera = make_shared<Camera>();

    const Json::Value jsonCamera = jsonScene["camera"];
    if (!jsonCamera.empty()) {
        camera->setPosition(loadVector3f(jsonCamera["position"]));
        camera->setLookAt(loadVector3f(jsonCamera["lookAt"]));
        camera->setUp(loadVector3f(jsonCamera["up"]));

        const Json::Value jsonProjection = jsonCamera["projection"];
        camera->setProjection(
            jsonProjection["fov"].asFloat(),
            jsonProjection["aspect"].asFloat(),
            jsonProjection["nearZ"].asFloat(),
            jsonProjection["farZ"].asFloat()
        );
    }
    else {
        camera->setPosition(0.0F, 0.0F, -100.0F);
        camera->setLookAt(0.0F, 0.0F, 0.0F);
        camera->setUp(0.0F, 1.0F, 0.0F);
        camera->setProjection(45.0F, 1.0F, 0.1F, 10000.0F);
    }

    scene->setCamera(camera);
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::loadLights()
{
    Json::Value jsonLightDefinitions = jsonScene["lights"];
    const LightDefinitionDeserializer deserializer;

    for (const auto& jsonLightDefinition : jsonLightDefinitions) {
        shared_ptr<Light> light = deserializer.deserialize(jsonLightDefinition);
        scene->add(light);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::loadModels()
{
    const ModelDefinitionDeserializer deserializer(effectDefaults);
    Json::Value jsonModelDefinitions = jsonScene["models"];

    for (const auto& jsonModelDefinition : jsonModelDefinitions) {
        ModelDefinition modelDefinition = deserialize(jsonModelDefinition, deserializer);
        const shared_ptr<Effect> effect = loadEffect(modelDefinition.effect);
        const shared_ptr<Mesh> mesh = loadModel(modelDefinition, effect);
        attachToSceneGraph(mesh, modelDefinition);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ModelDefinition SceneLoader::deserialize(const Json::Value& jsonModelDefinition,
    const ModelDefinitionDeserializer& deserializer) const
{
    return deserializer.deserialize(jsonModelDefinition);
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Effect> SceneLoader::loadEffect(const EffectDefinition& effectDefinition)
{
    const ShaderLoader shaderLoader(graphicsDevice);
    shared_ptr<VertexShader> vertexShader =
        shaderLoader.loadVertexShader(effectDefinition.vertexShaderPath, "main", effectDefinition.vertexFormat);
    shared_ptr<FragmentShader> fragmentShader =
        shaderLoader.loadFragmentShader(effectDefinition.fragmentShaderPath, "main");
    unique_ptr<ShaderProgram> shaderProgram =
        make_unique<ShaderProgram>(vertexShader, fragmentShader);

    vector<shared_ptr<Texture2D>> textures;
    const Texture2DLoader textureLoader(graphicsDevice);
    for (const string& texturePath : effectDefinition.texturePaths) {
        textures.push_back(textureLoader.load(texturePath));
    }

    const shared_ptr<Effect> effect = createEffect(effectDefinition, shaderProgram, textures);
    effect->updateFrom(scene->getLights());
    effect->updateFrom(make_shared<Material>(effectDefinition.material));

    effects.push_back(effect);

    return effects.back();
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Effect> SceneLoader::createEffect(const EffectDefinition& effectDefinition,
    unique_ptr<ShaderProgram>& shaderProgram,
    const vector<shared_ptr<Texture2D>>& textures)
{
    if (effectDefinition.id == VertexColorEffect::ID) {
        return make_shared<VertexColorEffect>(graphicsDevice, renderPass, shaderProgram);
    }

    if (effectDefinition.id == Texture2DEffect::ID) {
        return make_shared<Texture2DEffect>(graphicsDevice, renderPass, shaderProgram, textures[0]);
    }

    if (effectDefinition.id == DirectionalLightEffect::ID) {
        return make_shared<DirectionalLightEffect>(graphicsDevice, renderPass, shaderProgram);
    }

    RFX_NOT_IMPLEMENTED();
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Mesh> SceneLoader::loadModel(const ModelDefinition& modelDefinition,
    const shared_ptr<Effect>& effect) const
{
    ModelLoader modelLoader(graphicsDevice);
    return modelLoader.load(modelDefinition.modelPath, effect);
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneLoader::attachToSceneGraph(const shared_ptr<Mesh>& mesh,
    const ModelDefinition& modelDefinition) const
{
    unique_ptr<SceneNode> sceneNode = make_unique<SceneNode>();

    const Transform& transform = modelDefinition.transform;
    if (!transform.isIdentity()) {
        Transform& localTransform = sceneNode->getLocalTransform();
        localTransform.setTranslation(transform.getTranslation());
        localTransform.setScale(transform.getScale());
        localTransform.setRotation(transform.getRotation());
        localTransform.update();

        sceneNode->updateWorldTransform();

        mesh->getEffect()->setModelMatrix(sceneNode->getWorldTransform().getMatrix());
    }

    sceneNode->attach(mesh);
    scene->add(sceneNode);
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<Effect>>& SceneLoader::getEffects() const
{
    return effects;
}

// ---------------------------------------------------------------------------------------------------------------------
