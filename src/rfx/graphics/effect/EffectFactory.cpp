#include "rfx/pch.h"
#include "rfx/graphics/effect/EffectFactory.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

EffectFactory::EffectFactory(const string& effectId)
    : effectId(effectId) {}

// ---------------------------------------------------------------------------------------------------------------------

const string& EffectFactory::getEffectId() const
{
    return effectId;
}

// ---------------------------------------------------------------------------------------------------------------------
