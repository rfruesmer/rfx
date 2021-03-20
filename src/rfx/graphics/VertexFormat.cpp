#include "rfx/pch.h"
#include "rfx/graphics/VertexFormat.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

VertexFormat::VertexFormat()
    : VertexFormat(COORDINATES) {}

// ---------------------------------------------------------------------------------------------------------------------

VertexFormat::VertexFormat(
    uint32_t formatMask,
    uint32_t texCoordSetCount)
{
    RFX_CHECK_ARGUMENT(formatMask & COORDINATES);

    formatMask_ = formatMask;

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
    : VertexFormat(theOther.getFormatMask(), theOther.getTexCoordSetCount()) {}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t VertexFormat::getFormatMask() const
{
    return formatMask_;
}

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

bool VertexFormat::operator==(const VertexFormat& rhs) const
{
    return formatMask_ == rhs.formatMask_
        && texCoordSetCount_ == rhs.texCoordSetCount_;
}

// ---------------------------------------------------------------------------------------------------------------------
