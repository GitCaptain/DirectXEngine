#include "Graphics.h"

bool Graphics::initialize(HWND hwnd, int width, int height) {
    
    windowHeight = height;
    windowWidth = width;

    if (!initializeDirectX(hwnd)) {
        return false;
    }

    state.device = device.Get();
    state.deviceContext = deviceContext.Get();
    state.windowHeight = &windowHeight;
    state.windowWidth = &windowWidth;

    if (!initializeScene()) {
        return false;
    }

#ifdef ENABLE_IMGUI
    // setup ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());
    ImGui::StyleColorsDark();
#endif

    fpsTimer.startTimer();

    return true;
}

void Graphics::renderFrame() {

    float bgcolor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    deviceContext->ClearRenderTargetView(renderTargetView.Get(), bgcolor);
    deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); 
    
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->RSSetState(rasterizerState.Get());
    deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
    deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());

    renderScene.render();

    //Draw text
    static int fpsCounter = 0;
    fpsCounter += 1;
    static std::wstring fpsString = L"FPS: 0";
    if (fpsTimer.getMillisecondsElapsed() > 1000.0f) {
        fpsString = L"FPS: " + std::to_wstring(fpsCounter);
        fpsCounter = 0;
        fpsTimer.restartTimer(); 
    }
    spriteBatch->Begin(); 
    spriteFont->DrawString(spriteBatch.get(), fpsString.c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
    spriteBatch->End();

    const UINT vsync = 0;
    swapChain->Present(vsync, 0);
}

bool Graphics::initializeDirectX(HWND hwnd) {
    try {
        auto adapters = AdapterReader::getAdapters();
        ONFAILLOG(adapters.empty(), "No DXGI Adapters found.", false);

        // take adapter with maximum video memory
        auto bestAdapter = &adapters[0];
        for (const auto& adp : adapters) {
            if (adp.description.DedicatedVideoMemory > bestAdapter->description.DedicatedVideoMemory) {
                bestAdapter = const_cast<AdapterData*>(&adp);
            }
        }

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
        COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");

        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
        hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
        COM_ERROR_IF_FAILED(hr, "GetBuffer failed.");

        hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");

        // Describe our depth/stencil buffer
        CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, windowWidth, windowHeight);
        depthStencilTextureDesc.MipLevels = 1;
        depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        hr = device->CreateTexture2D(&depthStencilTextureDesc, nullptr, depthStencilBuffer.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, "Failed to create depth Stencil buffer.");

        hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, depthStencilView.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, "Failed to create depth Stencil view.");

        deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

        // Create depth stencil state
        CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

        hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, "Failed to create depth Stencil view.");

        // create and set viewport
        CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));  
        deviceContext->RSSetViewports(1, &viewport);

        // Create rasterizer state
        CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
        hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

        // Create rasterizer state for culling front
        CD3D11_RASTERIZER_DESC rasterizerDescCullFront(D3D11_DEFAULT);
        rasterizerDescCullFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
        hr = device->CreateRasterizerState(&rasterizerDescCullFront, rasterizerStateCullFront.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state for cull front.");

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

        hr = device->CreateBlendState(&blendDesc, blendState.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, "Failed to create blend state");


        spriteBatch = std::make_unique<DirectX::SpriteBatch>(deviceContext.Get());
        spriteFont = std::make_unique<DirectX::SpriteFont>(device.Get(), L"Resources\\Fonts\\comic_sans_ms_16.spritefont");

        // Create sampler description for sampler state
        CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        // Create sampler state
        hr = device->CreateSamplerState(&sampDesc, samplerState.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");
    }
    catch (const COMException& e) {
        ErrorLogger::log(e);
        return false;
    }
    return true;
}

bool Graphics::initializeScene() {
    return renderScene.initialize(state);
}

const GraphicsState& Graphics::getGraphicsState() const {
    return state;
}
