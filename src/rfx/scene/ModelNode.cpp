#include "rfx/pch.h"
#include "rfx/scene/ModelNode.h"
#include "rfx/scene/PointLight.h"
#include "rfx/scene/SpotLight.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

ModelNode::ModelNode(std::shared_ptr<ModelNode> parent)
    : parent_(parent.get()) {} // TODO: use weak_ptr?

// ---------------------------------------------------------------------------------------------------------------------

ModelNode* ModelNode::getParent() const
{
    return parent_;
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelNode::addChild(shared_ptr<ModelNode> child)
{
    children_.push_back(move(child));
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelNode::setChildren(const vector<shared_ptr<ModelNode>>& children)
{
    children_ = children;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<ModelNode>>& ModelNode::getChildren() const
{
    return children_;
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelNode::setLocalTransform(const mat4& localTransform)
{
    localTransform_ = localTransform;
}

// ---------------------------------------------------------------------------------------------------------------------

const mat4& ModelNode::getLocalTransform() const
{
    return localTransform_;
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelNode::setWorldTransform(const mat4& worldTransform)
{
    worldTransform_ = worldTransform;
}

// ---------------------------------------------------------------------------------------------------------------------

const mat4& ModelNode::getWorldTransform() const
{
    return worldTransform_;
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelNode::setMeshes(const vector<shared_ptr<Mesh>>& meshes)
{
    meshes_ = meshes;
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelNode::addMesh(shared_ptr<Mesh> mesh)
{
    meshes_.push_back(move(mesh));
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<Mesh>>& ModelNode::getMeshes() const
{
    return meshes_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ModelNode::getMeshCount() const
{
    return meshes_.size();
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelNode::addLight(std::shared_ptr<Light> light)
{
    lights_.push_back(move(light));
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelNode::compile()
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

void ModelNode::updateLights()
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

