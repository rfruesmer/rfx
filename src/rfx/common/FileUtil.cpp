#include "rfx/pch.h"
#include "rfx/common/FileUtil.h"

using namespace rfx;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

void FileUtil::readTextFile(const filesystem::path& filePath, string& outFile)
{
    const path absolutePath =
        filePath.is_absolute() ? filePath : current_path() / filePath;

    const ifstream inputStream(absolutePath.string(), ifstream::binary);
    ostringstream outputStream;
    outputStream << inputStream.rdbuf();
    outFile = outputStream.str();
}

// ---------------------------------------------------------------------------------------------------------------------
