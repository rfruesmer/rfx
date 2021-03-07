#include "rfx/pch.h"
#include "rfx/scene/SceneNode.h"
#include "rfx/scene/PointLight.h"
#include "rfx/scene/SpotLight.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

SceneNode::SceneNode(std::shared_ptr<SceneNode> parent)
    : parent_(parent.get()) {} // TODO: use weak_ptr?

// ---------------------------------------------------------------------------------------------------------------------

SceneNode* SceneNode::getParent() const
{
    return parent_;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::addChild(shared_ptr<SceneNode> child)
{
    children_.push_back(move(child));
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::setChildren(const vector<shared_ptr<SceneNode>>& children)
{
    children_ = children;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<SceneNode>>& SceneNode::getChildren() const
{
    return children_;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::setLocalTransform(const mat4& localTransform)
{
    localTransform_ = localTransform;
}

// ---------------------------------------------------------------------------------------------------------------------

const mat4& SceneNode::getLocalTransform() const
{
    return localTransform_;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::setWorldTransform(const mat4& worldTransform)
{
    worldTransform_ = worldTransform;
}

// ---------------------------------------------------------------------------------------------------------------------

const mat4& SceneNode::getWorldTransform() const
{
    return worldTransform_;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::setMeshes(const vector<shared_ptr<Mesh>>& meshes)
{
    meshes_ = meshes;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::addMesh(shared_ptr<Mesh> mesh)
{
    meshes_.push_back(move(mesh));
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<Mesh>>& SceneNode::getMeshes() const
{
    return meshes_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t SceneNode::getMeshCount() const
{
    return meshes_.size();
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::addLight(std::shared_ptr<Light> light)
{
    lights_.push_back(move(light));
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::compile()
{
    if (parent_) {
        worldTransform_ = parent_->getWorldTransform() * localTransform_;
    }

    if (!lights_.empty()) {
        updateLights();
    }

    for (const auto& child : children_) {
        child->compile();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::updateLights()
{
//    const vec3 worldPosition = vec3(worldTransform_[3]);

    quat orientation;
    vec3 position;
    vec3 unused;
    vec4 unused2;
    decompose(worldTransform_, unused, orientation, position, unused, unused2);

    vec3 direction = normalize(orientation * vec3(0.0f, 0.0f, -1.0f));

    for (const auto& light : lights_) {
        switch (light->getType()) {
        case Light::POINT:
            static_pointer_cast<PointLight>(light)->setPosition(position);
            break;
        case Light::SPOT:
            static_pointer_cast<SpotLight>(light)->setPosition(position);
            static_pointer_cast<SpotLight>(light)->setDirection(direction);
            break;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

