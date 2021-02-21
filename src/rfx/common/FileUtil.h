#pragma once


namespace rfx {

class FileUtil
{
public:
    FileUtil() = delete;

    static void readTextFile(const std::filesystem::path& filePath, std::string& outFile);

    static void readBinaryFile(const std::filesystem::path& filePath, size_t byteCount, void* dest);
};

} // namespace rfx