#pragma once

namespace rfx {

class VertexFormat
{
public:
    static const int COORDINATES = 1;
    static const int COLORS_3 = 2;
    static const int COLORS_4 = 4;
    static const int NORMALS = 8;
    static const int TEXCOORDS = 16;
    static const int TANGENTS = 32;

    VertexFormat();
    explicit VertexFormat(unsigned int formatMask);
    VertexFormat(const VertexFormat& theOther);

    [[nodiscard]]
    uint32_t getVertexSize() const;

    [[nodiscard]]
    bool containsCoordinates() const;

    [[nodiscard]]
    bool containsColors3() const;

    [[nodiscard]]
    bool containsColors4() const;

    [[nodiscard]]
    bool containsNormals() const;

    [[nodiscard]]
    bool containsTexCoords() const;

    [[nodiscard]]
    bool containsTangents() const;

private:
    uint32_t vertexSize = 0;
    bool coordinates = false;
    bool colors3 = false;
    bool colors4 = false;
    bool normals = false;
    bool texCoords = false;
    bool tangents = false;
};

} // namespace rfx
