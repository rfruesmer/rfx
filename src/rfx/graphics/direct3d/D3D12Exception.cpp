#include "rfx/pch.h"
#include "rfx/graphics/direct3d/D3D12Exception.h"
#include "rfx/core/StringUtil.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

D3D12Exception::D3D12Exception(HRESULT hr, const char* function, const char* file, int line)
{
	const _com_error comError(hr);
	const string ansiComErrorMessage = StringUtil::convertWideToAnsiString(comError.ErrorMessage());

	ostringstream oss;
	oss << function << " failed in " << file << "(" << line << "): " << ansiComErrorMessage;

	message = oss.str();
}

// ---------------------------------------------------------------------------------------------------------------------

char const* D3D12Exception::what() const
{
	return message.c_str();
}

// ---------------------------------------------------------------------------------------------------------------------
