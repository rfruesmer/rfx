#pragma once

namespace rfx {

class StringUtil
{
public:
    StringUtil() = delete;

    static std::string trimLeft(const std::string& str);
    static std::string trimRight(const std::string& str);
    static std::string remove(const std::string& str, char charToRemove);
};

} // namespace rfx