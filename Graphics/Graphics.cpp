#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height) {
    
    if (!InitializeDirectX(hwnd, width, height)) {
        return false;
    }

    return true;
}

bool Graphics::InitializeDirectX(HWND hwnd, int width, int height) {
    
    auto adapters = AdapterReader::getAdapters(); 
    //HRESULT hr = D3D11CreateDeviceAndSwapChain();
    return true;
}
