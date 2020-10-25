#include "StringConverter.h"

std::wstring StringConverter::stringToWide(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}
