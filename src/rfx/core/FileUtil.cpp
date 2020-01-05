#include <rfx/pch.h>
#include <rfx/core/FileUtil.h>

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

void FileUtil::readTextFile(const filesystem::path& path, string& outFile)
{
    const filesystem::path finalPath = 
        path.is_absolute() ? path : filesystem::current_path() / path;

    const ifstream inputStream(finalPath.string(), ifstream::binary);
    ostringstream outputStream;
    outputStream << inputStream.rdbuf();
    outFile = outputStream.str();
}

// ---------------------------------------------------------------------------------------------------------------------
