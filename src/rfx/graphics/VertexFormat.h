#pragma once

namespace rfx
{
class VertexFormat
{
public:
    static const int COORDINATES = 1;
    static const int COLORS = 2;
    static const int NORMALS = 4;
    static const int TEXTURE_COORDINATES_2 = 8;
    static const int TEXTURE_COORDINATES_3 = 16;

    explicit VertexFormat(int formatMask);
    VertexFormat(const VertexFormat& theOther);

    uint32_t getVertexSize() const;

    bool containsCoordinates() const;
    bool containsNormals() const;
    bool containsColors() const;
    bool containsTexCoords() const;

private:
    uint32_t vertexSize = 0;
    bool coordinates = false;
    bool colors = false;
    bool normals = false;
    bool texCoords = false;
};
}
