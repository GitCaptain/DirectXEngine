#include "Graphics.h"

bool Graphics::initialize(HWND hwnd, int width, int height) {
    
    if (!initializeDirectX(hwnd, width, height)) {
        return false;
    }

    if (!initializeShaders()) {
        return false;
    }

    if (!initializeScene()) {
        return false;
    }

    return true;
}

void Graphics::renderFrame() {
    float bgcolor[] = {0.0f, 0.0f, 1.0f, 1.0f};
    deviceContext->ClearRenderTargetView(renderTargetView.Get(), bgcolor);
    
    deviceContext->IASetInputLayout(vertexShader.getInputLayout());
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->VSSetShader(vertexShader.getShader(), nullptr, 0);
    deviceContext->PSSetShader(pixelShader.getShader(), nullptr, 0);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

    deviceContext->Draw(3, 0);

    const UINT vsync = 1;
    swapChain->Present(vsync, 0);
}

bool Graphics::initializeDirectX(HWND hwnd, int width, int height) {
    
    auto adapters = AdapterReader::getAdapters(); 

    if (adapters.empty()) {
        ErrorLogger::log("No DXGI Adapters found.");
        return false;
    }

    // take adapter with maximum video memory
    auto bestAdapter = &adapters[0];
    for (const auto& adp : adapters) {
        if (adp.description.DedicatedVideoMemory > bestAdapter->description.DedicatedVideoMemory) {
            bestAdapter = const_cast<AdapterData*>(&adp);
        }
    }

    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    scd.BufferDesc.Width = width;
    scd.BufferDesc.Height = height;
    // TODO: set real numerator and denominator
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 1;
    
    scd.OutputWindow = hwnd;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    

    HRESULT hr = D3D11CreateDeviceAndSwapChain(bestAdapter->pAdapter,
                                               D3D_DRIVER_TYPE_UNKNOWN, 
                                                 nullptr, 
                                               0,
                                               nullptr,
                                               0,
                                               D3D11_SDK_VERSION,
                                               &scd,
                                               swapChain.GetAddressOf(),
                                               device.GetAddressOf(),
                                               nullptr,
                                               deviceContext.GetAddressOf());

    if (FAILED(hr)) {
        ErrorLogger::log(hr, "Failed to create device and swapchain.");
        return false;
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr)) {
        ErrorLogger::log(hr, "GetBuffer failed.");
        return false;
    }

    hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.GetAddressOf());

    if (FAILED(hr)) {
        ErrorLogger::log(hr, "Failed to create render target view.");
        return false;
    }

    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);

    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = width;
    viewport.Height = height;

    deviceContext->RSSetViewports(1, &viewport);

    return true;

}

bool Graphics::initializeShaders() {

    std::wstring shaderFolder = L"";

#pragma region DetermineShaderPath
    if (IsDebuggerPresent() == TRUE) {
#ifdef _DEBUG
    #ifdef _WIN64
        shaderFolder = L"x64\\Debug\\";
    #else
        shaderFolder = L"Debug\\";
    #endif
#else
    #ifdef _WIN64
        shaderFolder = L"x64\\Release\\";
    #else
        shaderFolder = L"Release\\";
    #endif
#endif
    }

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    UINT numElements = ARRAYSIZE(layout);

    if (!vertexShader.initialize(device, shaderFolder + L"vertexshader.cso", layout, numElements)) {
        return false;
    }
    
    if (!pixelShader.initialize(device, shaderFolder + L"pixelshader.cso")) {
        return false;
    }

    return true;
}

bool Graphics::initializeScene() {

    Vertex v[] = {
        Vertex(0.0f, 0.0f),
        Vertex(0.0f, -0.1f),
        Vertex(-0.1f, 0.0f),
        Vertex(0.1f, 0.0f),
    };

    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(v);
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    // TODO: change if some cpu operations needed
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData;
    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));

    vertexBufferData.pSysMem = v;

    HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());

    if (FAILED(hr)) {
        ErrorLogger::log(hr, "Failed to create vertex buffer.");
        return false;
    }

    return true;

}
