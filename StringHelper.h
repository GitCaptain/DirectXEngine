#pragma once

#include <string>

class StringHelper {
public:
    static std::wstring stringToWide(const std::string& str);
    static std::string getDirectoryFromPath(const std::string& filePath);
    static std::string getfileExtension(const std::string& fileName);
};