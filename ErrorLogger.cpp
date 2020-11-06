#include <comdef.h>
#include "ErrorLogger.h"
#include "StringHelper.h"

void ErrorLogger::log(const std::string& message) {
    std::string error_message = "Error: " + message;
    MessageBoxA(nullptr, error_message.c_str(), "Error", MB_ICONERROR);
}

void ErrorLogger::log(HRESULT hr, const std::string& message) {
    log(hr, StringHelper::stringToWide(message));
}

void ErrorLogger::log(HRESULT hr, const std::wstring& message) {
    _com_error error(hr);
    std::wstring error_message = L"Error: " + message + L"\n" + error.ErrorMessage();
    MessageBoxW(nullptr, error_message.c_str(), L"Error", MB_ICONERROR);
}

void ErrorLogger::log(const COMException& exception) {
    std::wstring error_message = exception.what();
    MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}
