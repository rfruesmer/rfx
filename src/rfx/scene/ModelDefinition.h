#pragma once

#include "rfx/graphics/VertexFormat.h"

namespace rfx
{

class ModelDefinition
{
public:
    ModelDefinition(const std::filesystem::path& modelPath, 
        const VertexFormat& vertexFormat,
        const std::filesystem::path& vertexShaderPath, 
        const std::filesystem::path& fragmentShaderPath,
        const std::filesystem::path& texturePath);

    void setModelPath(const std::filesystem::path& path);
    const std::filesystem::path& getModelPath() const;

    void setVertexFormat(const VertexFormat& format);
    const VertexFormat& getVertexFormat() const;

    void setVertexShaderPath(const std::filesystem::path& path);
    const std::filesystem::path& getVertexShaderPath() const;

    void setFragmentShaderPath(const std::filesystem::path& path);
    const std::filesystem::path& getFragmentShaderPath() const;

    void setTexturePath(const std::filesystem::path& path);
    const std::filesystem::path& getTexturePath() const;

private:
    std::filesystem::path modelPath;
    VertexFormat vertexFormat;
    std::filesystem::path vertexShaderPath;
    std::filesystem::path fragmentShaderPath;
    std::filesystem::path texturePath;
};    

} // namespace rfx
