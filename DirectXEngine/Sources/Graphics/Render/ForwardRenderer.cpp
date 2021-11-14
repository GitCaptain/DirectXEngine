#include "ForwardRenderer.h"
#include <vector>

AdapterData ForwardRenderer::selectAdapter() {
    std::vector<AdapterData> adapters = AdapterReader::getAdapters();
    if (adapters.empty()) {
        throw AdapterException("No DXGI Adapters found.");
    }
    // take adapter with maximum video memory
    AdapterData* bestAdapter = &adapters[0];
    for (const auto& adp : adapters) {
        if (adp.description.DedicatedVideoMemory > bestAdapter->description.DedicatedVideoMemory) {
            bestAdapter = const_cast<AdapterData*>(&adp);
        }
    }

    return *bestAdapter;
}

void ForwardRenderer::createRenderTargetView() {
    ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), static_cast<void**>(&backBuffer));
    COM_ERROR_IF_FAILED(hr, "GetBuffer failed.");

    hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);
    COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");
}

void ForwardRenderer::createDepthStencilBuffer() {
    // Describe our depth/stencil buffer
    CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, windowWidth, windowHeight);
    depthStencilTextureDesc.MipLevels = 1;
    depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    HRESULT hr = device->CreateTexture2D(&depthStencilTextureDesc, nullptr, &depthStencilBuffer);
    COM_ERROR_IF_FAILED(hr, "Failed to create depth Stencil buffer.");

    hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, &depthStencilView);
    COM_ERROR_IF_FAILED(hr, "Failed to create depth Stencil view.");
}

void ForwardRenderer::createDeviceAndSwapChain() {
    AdapterData bestAdapter = selectAdapter();

    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    scd.BufferDesc.Width = windowWidth;
    scd.BufferDesc.Height = windowHeight;
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

    scd.OutputWindow = renderWindowHandle;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(bestAdapter.pAdapter,
        D3D_DRIVER_TYPE_UNKNOWN,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &scd,
        &swapChain,
        &device,
        nullptr,
        &deviceContext);
    COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");

}

void ForwardRenderer::createAndSetRenderTargets() {
    createRenderTargetView();
    createDepthStencilBuffer();
    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
}

void ForwardRenderer::createDepthStencilState() {
    // Create depth stencil state
    CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

    HRESULT hr = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
    COM_ERROR_IF_FAILED(hr, "Failed to create depth Stencil view.");
}

void ForwardRenderer::createAndSetViewPort() {
    // create and set viewport
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));
    deviceContext->RSSetViewports(1, &viewport);
}

void ForwardRenderer::createRasterizerState() {
    // Create rasterizer state
    CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
    HRESULT hr = device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");
}

void ForwardRenderer::createBlendState() {
    // Create blend state
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));

    D3D11_RENDER_TARGET_BLEND_DESC rtbd;
    ZeroMemory(&rtbd, sizeof(rtbd));

    rtbd.BlendEnable = true;
    rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
    rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
    rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
    rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
    rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
    rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
    rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

    blendDesc.RenderTarget[0] = rtbd;

    HRESULT hr = device->CreateBlendState(&blendDesc, &blendState);
    COM_ERROR_IF_FAILED(hr, "Failed to create blend state");
}

void ForwardRenderer::createSamplerState() {
    // Create sampler description for sampler state
    CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    // Create sampler state
    HRESULT hr = device->CreateSamplerState(&sampDesc, &samplerState);
    COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
}

void ForwardRenderer::fillGraphicsState() {
    graphicsState.windowHeight = windowHeight;
    graphicsState.windowWidth = windowWidth;
    graphicsState.device = device.Get();
    graphicsState.deviceContext = deviceContext.Get();
}

bool ForwardRenderer::initRenderer(HWND renderWindowHandle, int windowWidth, int windowHeight) {

    this->renderWindowHandle = renderWindowHandle;
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;

    try {
        createDeviceAndSwapChain();
        createAndSetRenderTargets();
        createDepthStencilState();
        createAndSetViewPort();
        createRasterizerState();
        createBlendState();
        createSamplerState();
        fillGraphicsState();
    }
    catch (const COMException& e) {
        ErrorLogger::log(e);
        return false;
    }

    return true;
}

GraphicsState& ForwardRenderer::getGraphicsState() {
    return graphicsState;
}

void ForwardRenderer::preparePipeline() {
    float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView.Get(), bgcolor);
    deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->RSSetState(rasterizerState.Get());
    deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
    deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
}

void ForwardRenderer::present(size_t syncInterval, size_t flags) {
    swapChain->Present(syncInterval, flags);
}
