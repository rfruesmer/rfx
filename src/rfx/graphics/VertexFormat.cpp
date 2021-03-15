#include "rfx/pch.h"
#include "rfx/graphics/VertexFormat.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

VertexFormat::VertexFormat()
    : VertexFormat(COORDINATES) {}

// ---------------------------------------------------------------------------------------------------------------------

VertexFormat::VertexFormat(unsigned int formatMask, unsigned int texCoordSetCount)
{
    RFX_CHECK_ARGUMENT(formatMask & COORDINATES);

    if (formatMask & COORDINATES) {
        coordinates_ = true;
        vertexSize_ += 12;
    }

    if (formatMask & COLORS_3) {
        colors3_ = true;
        vertexSize_ += 12;
    }
    else if (formatMask & COLORS_4) {
        colors4_ = true;
        vertexSize_ += 16;
    }

    if (formatMask & NORMALS) {
        normals_ = true;
        vertexSize_ += 12;
    }

    if (formatMask & TEXCOORDS) {
        texCoords_ = true;
        if (texCoordSetCount == 0) {
            texCoordSetCount = 1;
        }
        RFX_CHECK_STATE(texCoordSetCount <= MAX_TEXCOORDSET_COUNT, "Requested texture coord set count not supported yet!");
        texCoordSetCount_ = texCoordSetCount;
        vertexSize_ += 8 * texCoordSetCount;
    }

    if (formatMask & TANGENTS) {
        tangents_ = true;
        vertexSize_ += 16;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

VertexFormat::VertexFormat(const VertexFormat& theOther)
    : vertexSize_(theOther.vertexSize_),
      coordinates_(theOther.coordinates_),
      normals_(theOther.normals_),
      colors3_(theOther.colors3_),
      colors4_(theOther.colors4_),
      texCoords_(theOther.texCoords_),
      texCoordSetCount_(theOther.texCoordSetCount_),
      tangents_(theOther.tangents_) {}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t VertexFormat::getVertexSize() const
{
    return vertexSize_;
}

// ---------------------------------------------------------------------------------------------------------------------

bool VertexFormat::containsCoordinates() const
{
    return coordinates_;
}

// ---------------------------------------------------------------------------------------------------------------------

bool VertexFormat::containsNormals() const
{
    return normals_;
}

// ---------------------------------------------------------------------------------------------------------------------

bool VertexFormat::containsColors3() const
{
    return colors3_;
}

// ---------------------------------------------------------------------------------------------------------------------

bool VertexFormat::containsColors4() const
{
    return colors4_;
}

// ---------------------------------------------------------------------------------------------------------------------

bool VertexFormat::containsTexCoords() const
{
    return texCoords_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t VertexFormat::getTexCoordSetCount() const
{
    return texCoordSetCount_;
}

// ---------------------------------------------------------------------------------------------------------------------

bool VertexFormat::containsTangents() const
{
    return tangents_;
}

// ---------------------------------------------------------------------------------------------------------------------
