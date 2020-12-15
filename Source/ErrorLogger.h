#pragma once

#include <Windows.h>
#include <string>
#include "COMException.h"

#define ONFAILHRLOG(hr, str, ret) if(FAILED(hr)){ErrorLogger::log(hr, str, __FILE__, __FUNCTION__, __LINE__); return ret;}
#define ONFAILLOG(cond, str, ret) if(cond){ErrorLogger::log(str); return ret;}

class ErrorLogger {
public:
    static void log(const std::string& message);
    static void log(HRESULT hr, const std::string& message, 
                    const std::string& file,    
                    const std::string& function, 
                    int line);
    static void log(HRESULT hr, 
                    const std::wstring& message, 
                    const std::string& file, 
                    const std::string& function, 
                    int line);
    static void log(const COMException& exception);
    static void log(HRESULT hr, const std::string& message);
    static void log(HRESULT hr, const std::wstring& message);
};