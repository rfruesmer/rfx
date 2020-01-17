#pragma once

namespace rfx
{
class VertexFormat
{
public:
    static const int COORDINATES = 1;
    static const int COLORS = 2;
    static const int NORMALS = 4;
    static const int TEXCOORDS = 8;

    explicit VertexFormat(unsigned int formatMask);
    VertexFormat(const VertexFormat& theOther);

    uint32_t getVertexSize() const;

    bool containsCoordinates() const;
    bool containsColors() const;
    bool containsNormals() const;
    bool containsTexCoords() const;

private:
    uint32_t vertexSize = 0;
    bool coordinates = false;
    bool colors = false;
    bool normals = false;
    bool texCoords = false;
};
}
