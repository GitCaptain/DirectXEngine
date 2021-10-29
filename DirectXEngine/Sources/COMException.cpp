#include "COMException.h"
#include "Utils/StringHelper.h"

COMException::COMException(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line) {
    _com_error error(hr);
    whatmsg = L"Msg: " + StringHelper::stringToWide(std::string(msg)) + L"\n";
    whatmsg += error.ErrorMessage();
    whatmsg += L"\nFile: " + StringHelper::stringToWide(file);
    whatmsg += L"\nFunction: " + StringHelper::stringToWide(function);
    whatmsg += L"\nLine: " + std::to_wstring(line) + L"\n";
}

const wchar_t* COMException::what() const {
    return whatmsg.c_str();
}
