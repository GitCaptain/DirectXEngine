#pragma once

#include <Windows.h>
#include <string>
#include "COMException.h"

#define ONFAILHRLOG(hr, str, ret) if(FAILED(hr)){ErrorLogger::log(hr, str); return ret;}
#define ONFAILLOG(cond, str, ret) if(cond){ErrorLogger::log(str); return ret;}

class ErrorLogger {
public:
    static void log(const std::string& message);
    static void log(HRESULT hr, const std::string& message);
    static void log(HRESULT hr, const std::wstring& message);
    static void log(const COMException& exception);
};