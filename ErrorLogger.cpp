#include <comdef.h>
#include "ErrorLogger.h"
#include "StringConverter.h"

void ErrorLogger::log(const std::string& message) {
    std::string error_message = "Error: " + message;
    MessageBoxA(nullptr, error_message.c_str(), "Error", MB_ICONERROR);
}

void ErrorLogger::log(HRESULT hr, const std::string& message) {
    log(hr, StringConverter::stringToWide(message));
    //_com_error error(hr);
    //std::wstring error_message = L"Error: " + StringConverter::stringToWide(message) + L"\n" + error.ErrorMessage();
    //MessageBoxW(nullptr, error_message.c_str(), L"Error", MB_ICONERROR);
}

void ErrorLogger::log(HRESULT hr, const std::wstring& message) {
    _com_error error(hr);
    std::wstring error_message = L"Error: " + message + L"\n" + error.ErrorMessage();
    MessageBoxW(nullptr, error_message.c_str(), L"Error", MB_ICONERROR);
}
