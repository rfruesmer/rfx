#include "rfx/pch.h"
#include "rfx/graphics/VertexFormat.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

VertexFormat::VertexFormat()
    : VertexFormat(COORDINATES) {}

// ---------------------------------------------------------------------------------------------------------------------

VertexFormat::VertexFormat(unsigned int formatMask)
{
    RFX_CHECK_ARGUMENT(formatMask & COORDINATES);

    if (formatMask & COORDINATES) {
        coordinates = true;
        vertexSize += 12;
    }

    if (formatMask & COLORS_3) {
        colors3 = true;
        vertexSize += 12;
    }
    else if (formatMask & COLORS_4) {
        colors4 = true;
        vertexSize += 16;
    }

    if (formatMask & NORMALS) {
        normals = true;
        vertexSize += 12;
    }

    if (formatMask & TEXCOORDS) {
        texCoords = true;
        vertexSize += 8;
    }

    if (formatMask & TANGENTS) {
        tangents = true;
        vertexSize += 16;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

VertexFormat::VertexFormat(const VertexFormat& theOther)
    : vertexSize(theOther.vertexSize),
      coordinates(theOther.coordinates),
      normals(theOther.normals),
      colors3(theOther.colors3),
      colors4(theOther.colors4),
      texCoords(theOther.texCoords),
      tangents(theOther.tangents) {}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t VertexFormat::getVertexSize() const
{
    return vertexSize;
}

// ---------------------------------------------------------------------------------------------------------------------

bool VertexFormat::containsCoordinates() const
{
    return coordinates;
}

// ---------------------------------------------------------------------------------------------------------------------

bool VertexFormat::containsNormals() const
{
    return normals;
}

// ---------------------------------------------------------------------------------------------------------------------

bool VertexFormat::containsColors3() const
{
    return colors3;
}

// ---------------------------------------------------------------------------------------------------------------------

bool VertexFormat::containsColors4() const
{
    return colors4;
}

// ---------------------------------------------------------------------------------------------------------------------

bool VertexFormat::containsTexCoords() const
{
    return texCoords;
}

// ---------------------------------------------------------------------------------------------------------------------

bool VertexFormat::containsTangents() const
{
    return tangents;
}

// ---------------------------------------------------------------------------------------------------------------------
