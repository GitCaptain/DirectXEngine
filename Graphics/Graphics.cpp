#include "Graphics.h"

bool Graphics::initialize(HWND hwnd, int width, int height) {
    
    windowHeight = height;
    windowWidth = width;

    if (!initializeDirectX(hwnd)) {
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
    float bgcolor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    deviceContext->ClearRenderTargetView(renderTargetView.Get(), bgcolor);
    deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); 
    
    deviceContext->IASetInputLayout(vertexShader.getInputLayout());
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->RSSetState(rasterizerState.Get());
    deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
    deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
    deviceContext->VSSetShader(vertexShader.getShader(), nullptr, 0);
    deviceContext->PSSetShader(pixelShader.getShader(), nullptr, 0);

    UINT offset = 0;

    // Update constatnt buffer
    DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
    camera.adjustPosition(0.0f, 0.01f, 0.0f);
    constantBuffer.data.mat = worldMatrix * camera.getViewMatrix() * camera.getProjectionMatrix();
    constantBuffer.data.mat = DirectX::XMMatrixTranspose(constantBuffer.data.mat);
    if (!constantBuffer.applyChanges()) {
        return;
    }

    deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    // Draw square
    deviceContext->PSSetShaderResources(0, 1, myTexture.GetAddressOf());
    deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.getStridePtr(), &offset);
    deviceContext->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    deviceContext->DrawIndexed(indicesBuffer.getBufferSize(), 0, 0);
    
    //Draw text
    spriteBatch->Begin();
    spriteFont->DrawString(spriteBatch.get(), L"HELLO WORLD", DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
    spriteBatch->End();

    const UINT vsync = 1;
    swapChain->Present(vsync, 0);
}

bool Graphics::initializeDirectX(HWND hwnd) {
    
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

    D3D11_TEXTURE2D_DESC depthStencilDesc;

    depthStencilDesc.Width = windowWidth;
    depthStencilDesc.Height = windowHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    hr = device->CreateTexture2D(&depthStencilDesc, nullptr, depthStencilBuffer.GetAddressOf());
    if (FAILED(hr)) {
        ErrorLogger::log(hr, "Failed to create depth Stencil buffer.");
        return false;
    }

    hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, depthStencilView.GetAddressOf());
    if (FAILED(hr)) {
        ErrorLogger::log(hr, "Failed to create depth Stencil view.");
        return false;
    }

    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get() );

    // Create  depth stencil state

    D3D11_DEPTH_STENCIL_DESC depthstencildesc;
    ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

    depthstencildesc.DepthEnable = true;
    depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
    depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

    hr = device->CreateDepthStencilState(&depthstencildesc, depthStencilState.GetAddressOf());
    if (FAILED(hr)) {
        ErrorLogger::log(hr, "Failed to create depth Stencil view.");
        return false;
    }


    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = windowWidth;
    viewport.Height = windowHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    // Set the viewport
    deviceContext->RSSetViewports(1, &viewport);

    // Create rasterizer state
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

    rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    // Render object whether it is in font of us or not
    rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

    hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
    
    if (FAILED(hr)) {
        ErrorLogger::log(hr, "Failed to create rasterizer state.");
        return false;
    }

    spriteBatch = std::make_unique<DirectX::SpriteBatch>(deviceContext.Get());
    spriteFont = std::make_unique<DirectX::SpriteFont>(device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");

    // Create sampler description for sampler state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));

    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create sampler state
    hr = device->CreateSamplerState(&sampDesc, samplerState.GetAddressOf());

    if (FAILED(hr)) {
        ErrorLogger::log(hr, "Failed to create rasterizer state.");
        return false;
    }

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
        {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
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
        Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f),
        Vertex(-0.5f,  0.5f, 0.0f, 0.0f, 0.0f),
        Vertex( 0.5f,  0.5f, 0.0f, 1.0f, 0.0f),
        Vertex( 0.5f, -0.5f, 0.0f, 1.0f, 1.0f),
    };

    // Load vertex data
    HRESULT hr = vertexBuffer.initialize(device.Get(), v, ARRAYSIZE(v));
    ONFAILHRLOG(hr, "Failed to create vertex buffer.", false);

    DWORD indices[] = {
        0, 1, 2,
        0, 2, 3,
    };

    // Load index data
    hr = indicesBuffer.initialize(device.Get(), indices, ARRAYSIZE(indices));
    ONFAILHRLOG(hr, "Failed to create indices buffer.", false);

    // Load texture
    hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data\\Textures\\piano.png", nullptr, myTexture.GetAddressOf());
    ONFAILHRLOG(hr, "Failed to create wic texture from file", false);

    // Initialize constant buffer(s)
    hr = constantBuffer.initialize(device.Get(), deviceContext.Get());
    ONFAILHRLOG(hr, "Failed to initialize constant buffer.", false);

    camera.setPosition(0.0f, 0.0f, -2.0f);
    camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);
    return true;
}
