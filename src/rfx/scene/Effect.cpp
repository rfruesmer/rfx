#include "rfx/pch.h"
#include "rfx/scene/Effect.h"


using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

vector<string> Effect::buildShaderDefines(const shared_ptr<Material>& material, const VertexFormat& vertexFormat)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> Effect::buildVertexShaderInputs(const VertexFormat& vertexFormat)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> Effect::buildVertexShaderOutputs(const VertexFormat& vertexFormat)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------

vector<string> Effect::buildFragmentShaderInputs(const VertexFormat& vertexFormat)
{
    return vector<string>();
}

// ---------------------------------------------------------------------------------------------------------------------
