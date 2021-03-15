#pragma once

namespace rfx {

class VertexFormat
{
public:
    static const unsigned int COORDINATES = 1;
    static const unsigned int COLORS_3 = 2; // TODO: consolidate COLORS_3 & COLORS_4
    static const unsigned int COLORS_4 = 4;
    static const unsigned int NORMALS = 8;
    static const unsigned int TEXCOORDS = 16;
    static const unsigned int TANGENTS = 32;

    static const unsigned int MAX_TEXCOORDSET_COUNT = 8;

    VertexFormat();
    explicit VertexFormat(unsigned int formatMask, unsigned int texCoordSetCount = 0);
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
    uint32_t getTexCoordSetCount() const;

    [[nodiscard]]
    bool containsTangents() const;

private:
    uint32_t vertexSize_ = 0;

    bool coordinates_ = false;
    bool colors3_ = false;
    bool colors4_ = false;
    bool normals_ = false;
    bool texCoords_ = false;
    uint32_t texCoordSetCount_ = 0;
    bool tangents_ = false;
};

} // namespace rfx
