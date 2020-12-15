#include <comdef.h>
#include "ErrorLogger.h"
#include "StringHelper.h"

void ErrorLogger::log(const std::string& message) {
    std::string error_message = "Error: " + message;
    MessageBoxA(nullptr, error_message.c_str(), "Error", MB_ICONERROR);
}

void ErrorLogger::log(HRESULT hr, 
                      const std::string& message, 
                      const std::string& file,
                      const std::string& function, 
                      int line) {
    log(hr, StringHelper::stringToWide(message), file, function, line);
}

void ErrorLogger::log(HRESULT hr, 
                      const std::wstring& message, 
                      const std::string& file, 
                      const std::string& function, 
                      int line) {
    _com_error error(hr);
    std::wstring error_message = L"Error: " + message + L"\n" + error.ErrorMessage() + L"\n";
    error_message += L"At file: " + StringHelper::stringToWide(file) +
                     L", function: " + StringHelper::stringToWide(function) +
                     L", line: " + std::to_wstring(line);
    MessageBoxW(nullptr, error_message.c_str(), L"Error", MB_ICONERROR);
}

void ErrorLogger::log(const COMException& exception) {
    std::wstring error_message = exception.what();
    MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

void ErrorLogger::log(HRESULT hr, const std::string& message) {
    log(hr, StringHelper::stringToWide(message));
}

void ErrorLogger::log(HRESULT hr, const std::wstring& message) {
    _com_error error(hr);
    std::wstring error_message = L"Error: " + message + L"\n" + error.ErrorMessage();
    MessageBoxW(nullptr, error_message.c_str(), L"Error", MB_ICONERROR);
}
