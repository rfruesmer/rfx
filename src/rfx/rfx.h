#pragma once

#ifdef _WINDOWS
#include "res/Resource.h"
#endif // _WINDOWS

#include "rfx/core/Logger.h"

using handle_t = void*;

#ifdef _WINDOWS


#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = nullptr; } }
#endif

#endif // _WINDOWS
