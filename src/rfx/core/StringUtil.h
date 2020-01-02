#pragma once

namespace rfx
{

class StringUtil
{
public:
    StringUtil() = delete;

    static std::string convertWideToAnsiString(const std::wstring& wstr);
    static std::string convertWideToAnsiString(const wchar_t* wstr);
};

} // namespace rfx