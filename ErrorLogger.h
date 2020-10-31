#pragma once

#include <Windows.h>
#include <string>


#define ONFAILHRLOG(hr, str) if(FAILED(hr)){ErrorLogger::log(hr, str); return false;}
#define ONFAILLOG(str) 0;

class ErrorLogger {
public:
    static void log(const std::string& message);
    static void log(HRESULT hr, const std::string& message);
    static void log(HRESULT hr, const std::wstring& message);
};