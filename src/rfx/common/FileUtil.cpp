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
    RFX_CHECK_STATE(exists(absolutePath), "File not found: " + absolutePath.string());

    const ifstream inputStream(absolutePath.string(), ifstream::binary);
    ostringstream outputStream;
    outputStream << inputStream.rdbuf();
    outFile = outputStream.str();
}

// ---------------------------------------------------------------------------------------------------------------------

void FileUtil::readBinaryFile(const path& filePath, size_t byteCount, void* dest)
{
    const path absolutePath =
        filePath.is_absolute() ? filePath : current_path() / filePath;
    RFX_CHECK_STATE(exists(absolutePath), "File not found: " + absolutePath.string());
    ifstream inputStream(absolutePath.string(), ifstream::binary);
    inputStream.read(reinterpret_cast<char*>(dest), byteCount);
}

// ---------------------------------------------------------------------------------------------------------------------
