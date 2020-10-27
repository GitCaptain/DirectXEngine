#pragma once

#include <Windows.h>
#include <string>

class ErrorLogger {
public:
    static void log(const std::string& message);
    static void log(HRESULT hr, const std::string& message);
    static void log(HRESULT hr, const std::wstring& message);
};