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

    if (formatMask & COLORS) {
        colors = true;
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
}

// ---------------------------------------------------------------------------------------------------------------------

VertexFormat::VertexFormat(const VertexFormat& theOther)
    : vertexSize(theOther.vertexSize),
      coordinates(theOther.coordinates),
      normals(theOther.normals),
      colors(theOther.colors),
      texCoords(theOther.texCoords) {}

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

bool VertexFormat::containsColors() const
{
    return colors;
}

// ---------------------------------------------------------------------------------------------------------------------

bool VertexFormat::containsTexCoords() const
{
    return texCoords;
}

// ---------------------------------------------------------------------------------------------------------------------
