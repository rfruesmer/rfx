#pragma once

#include "rfx/scene/Scene.h"

namespace rfx
{

class SceneImporter
{
public:
    virtual ~SceneImporter() = default;

    virtual ScenePtr import(const std::filesystem::path& path) = 0;
};

} // namespace rfx
