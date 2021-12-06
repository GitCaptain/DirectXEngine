#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <exception>
#include "..\ErrorLogger.h"

class AdapterData {
public:
    AdapterData(IDXGIAdapter* pAdapter);
    IDXGIAdapter* pAdapter = nullptr;
    DXGI_ADAPTER_DESC description;
};

class AdapterReader {
public:
    static std::vector<AdapterData> getAdapters();
private:
    static std::vector<AdapterData> adapters;
};

struct AdapterException: public std::exception {
    using std::exception::exception;
};
