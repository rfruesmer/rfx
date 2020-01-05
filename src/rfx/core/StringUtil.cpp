#include "rfx/pch.h"
#include "rfx/core/StringUtil.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

string StringUtil::convertWideToAnsiString(const wstring& wstr)
{
    return convertWideToAnsiString(wstr.c_str());
}

// ---------------------------------------------------------------------------------------------------------------------

string StringUtil::convertWideToAnsiString(const wchar_t* wstr)
{
    const int wideCharCount = static_cast<int>(wcslen(wstr));
    const int multiByteCount = WideCharToMultiByte(CP_ACP, 0, wstr,
        static_cast<int>(wideCharCount), nullptr, 0, nullptr, nullptr);

    string multiByteString;
    multiByteString.resize(multiByteCount);

    WideCharToMultiByte(CP_ACP, 0, wstr, wideCharCount, &multiByteString[0], multiByteCount, nullptr, nullptr);

    return multiByteString;
}

// ---------------------------------------------------------------------------------------------------------------------

string StringUtil::trimLeft(const string& str)
{
    const size_t i = str.find_first_not_of(" \n\t");
    if (i == wstring::npos) {
        return str;
    }

    return str.substr(i, str.length() - i);
}

// ---------------------------------------------------------------------------------------------------------------------

string StringUtil::trimRight(const string& str)
{
    size_t i = str.find_last_not_of(" \n\t");
    if (i == wstring::npos) {
        return str;
    }
    return str.substr(0, i + 1);}

// ---------------------------------------------------------------------------------------------------------------------
