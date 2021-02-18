#include "rfx/pch.h"
#include "StringUtil.h"

using namespace rfx;
using namespace std;

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