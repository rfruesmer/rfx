#include "rfx/pch.h"
#include "rfx/scene/LightNode.h"
#include "rfx/scene/PointLight.h"
#include "rfx/scene/SpotLight.h"


using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

LightNode::LightNode(const NodePtr& parent)
    : Node(parent) {}

// ---------------------------------------------------------------------------------------------------------------------

void LightNode::addLight(LightPtr light)
{
    lights_.push_back(move(light));
}

// ---------------------------------------------------------------------------------------------------------------------

void LightNode::update()
{
    if (lights_.empty()) {
        return;
    }

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
        default:
            RFX_THROW_NOT_IMPLEMENTED();
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

