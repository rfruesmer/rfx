#pragma once

namespace rfx {

class VertexFormat
{
public:
    static const int COORDINATES = 1;
    static const int COLORS = 2;
    static const int NORMALS = 4;
    static const int TEXCOORDS = 8;

    VertexFormat();
    explicit VertexFormat(unsigned int formatMask);
    VertexFormat(const VertexFormat& theOther);

    [[nodiscard]]
    uint32_t getVertexSize() const;

    [[nodiscard]]
    bool containsCoordinates() const;

    [[nodiscard]]
    bool containsColors() const;

    [[nodiscard]]
    bool containsNormals() const;

    [[nodiscard]]
    bool containsTexCoords() const;

private:
    uint32_t vertexSize = 0;
    bool coordinates = false;
    bool colors = false;
    bool normals = false;
    bool texCoords = false;
};

} // namespace rfx
