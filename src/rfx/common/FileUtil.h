#pragma once


namespace rfx {

class FileUtil
{
public:
    FileUtil() = delete;

    static void readTextFile(const std::filesystem::path& filePath, std::string& outFile);
};

} // namespace rfx