#include "Graphics.h"

bool Graphics::initialize(HWND hwnd, int width, int height) {
    
    windowHeight = height;
    windowWidth = width;
    fpsTimer.startTimer();
    if (!initializeDirectX(hwnd)) {
        return false;
    }

    if (!initializeShaders()) {
        return false;
    }

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
    deviceContext->OMSetBlendState(blendState.Get(), nullptr, 0xFFFFFFFF);
    deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
    deviceContext->VSSetShader(vertexShader.getShader(), nullptr, 0);
    deviceContext->PSSetShader(pixelShader.getShader(), nullptr, 0);
 
    UINT offset = 0;

    static float alpha = 0.1f;
    { // cube
        // Update constant buffer
        DirectX::XMMATRIX worldMatrix = /*DirectX::XMMatrixScaling(5.0f, 5.0f, 5.0f) **/ DirectX::XMMatrixTranslation(0, 0, -1.0f);
        cb_vs_vertexshader.data.mat = worldMatrix * camera.getViewMatrix() * camera.getProjectionMatrix();
        cb_vs_vertexshader.data.mat = DirectX::XMMatrixTranspose(cb_vs_vertexshader.data.mat);
        if (!cb_vs_vertexshader.applyChanges()) { // ignore frame in case of errors
            return;
        }

        cb_ps_pixelshader.data.alpha = alpha;
        if (!cb_ps_pixelshader.applyChanges()) { // ignore frame in case of errors
            return;
        }

        deviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexshader.GetAddressOf());
        deviceContext->PSSetConstantBuffers(0, 1, cb_ps_pixelshader.GetAddressOf());

        // Draw square
        deviceContext->PSSetShaderResources(0, 1, pavementTexture.GetAddressOf());
        deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.getStridePtr(), &offset);
        deviceContext->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        deviceContext->RSSetState(rasterizerStateCullFront.Get());
        deviceContext->DrawIndexed(indicesBuffer.getBufferSize(), 0, 0);
        deviceContext->RSSetState(rasterizerState.Get());
        deviceContext->DrawIndexed(indicesBuffer.getBufferSize(), 0, 0);
    }

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

#ifdef ENABLE_IMGUI
    // start dear imgui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    // create imgui test window
    ImGui::Begin("Settings");
    ImGui::DragFloat("Texture alpha", &alpha, 0.01f, 0.0f, 1.0f);
    ImGui::End();
    // Assemble Together Draw data
    ImGui::Render();
    // render draw data
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

    const UINT vsync = 0;
    swapChain->Present(vsync, 0);
}

Camera::Camera& const Graphics::getCamera() {
    return camera;
}

bool Graphics::initializeDirectX(HWND hwnd) {
    
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
    ONFAILHRLOG(hr, "Failed to create device and swapchain.", false);
    
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    ONFAILHRLOG(hr, "GetBuffer failed.", false);

    hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.GetAddressOf());
    ONFAILHRLOG(hr, "Failed to create render target view.", false);

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
    ONFAILHRLOG(hr, "Failed to create depth Stencil buffer.", false);

    hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, depthStencilView.GetAddressOf());
    ONFAILHRLOG(hr, "Failed to create depth Stencil view.", false);

    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get() );

    // Create  depth stencil state
    D3D11_DEPTH_STENCIL_DESC depthstencildesc;
    ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

    depthstencildesc.DepthEnable = true;
    depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
    depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

    hr = device->CreateDepthStencilState(&depthstencildesc, depthStencilState.GetAddressOf());
    ONFAILHRLOG(hr, "Failed to create depth Stencil view.", false);
    
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
    rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

    hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
    ONFAILHRLOG(hr, "Failed to create rasterizer state.", false);

    // Create rasterizer state for culling front
    D3D11_RASTERIZER_DESC rasterizerDescCullFront;
    ZeroMemory(&rasterizerDescCullFront, sizeof(rasterizerDescCullFront));

    rasterizerDescCullFront.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    // Render object whether it is in font of us or not
    rasterizerDescCullFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;

    hr = device->CreateRasterizerState(&rasterizerDescCullFront, rasterizerStateCullFront.GetAddressOf());
    ONFAILHRLOG(hr, "Failed to create rasterizer state for cull front.", false);

    // Create blend state
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));

    D3D11_RENDER_TARGET_BLEND_DESC rtbd;
    ZeroMemory(&rtbd, sizeof(rtbd));

    rtbd.BlendEnable = true;
    rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
    rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
    rtbd.BlendOp =  D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
    rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
    rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
    rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
    rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
    
    blendDesc.RenderTarget[0] = rtbd;
    
    hr = device->CreateBlendState(&blendDesc, blendState.GetAddressOf());
    ONFAILHRLOG(hr, "Failed to create blend state", false);


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
    ONFAILHRLOG(hr, "Failed to create rasterizer state.", false);

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
        Vertex(-0.5f, -0.5f, -0.5f, 0.0f, 1.0f), // Front bottom left
        Vertex(-0.5f,  0.5f, -0.5f, 0.0f, 0.0f), // Front top left
        Vertex( 0.5f,  0.5f, -0.5f, 1.0f, 0.0f), // Front top right
        Vertex( 0.5f, -0.5f, -0.5f, 1.0f, 1.0f), // Front bottom right
        
        Vertex(-0.5f, -0.5f, 0.5f, 0.0f, 1.0f), // Back  bottom left
        Vertex(-0.5f,  0.5f, 0.5f, 0.0f, 0.0f), // Back top left
        Vertex( 0.5f,  0.5f, 0.5f, 1.0f, 0.0f), // Back top right
        Vertex( 0.5f, -0.5f, 0.5f, 1.0f, 1.0f), // Back bottom right
    };

    // Load vertex data
    HRESULT hr = vertexBuffer.initialize(device.Get(), v, ARRAYSIZE(v));
    ONFAILHRLOG(hr, "Failed to create vertex buffer.", false);

    DWORD indices[] = {
        0, 1, 2, // front
        0, 2, 3, // front
        4, 7, 6, // back
        4, 6, 5, // back
        3, 2, 6, // right
        3, 6, 7, // right
        4, 5, 1, // left
        4, 1, 0, // left
        1, 5, 6, // top
        1, 6, 2, // top
        0, 3, 7, // bottom
        0, 7, 4, // bottom
    };

    // Load index data
    hr = indicesBuffer.initialize(device.Get(), indices, ARRAYSIZE(indices));
    ONFAILHRLOG(hr, "Failed to create indices buffer.", false);

    // Load texture
    hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data\\Textures\\seamless_grass.jpg", nullptr, grassTexture.GetAddressOf());
    ONFAILHRLOG(hr, "Failed to create wic texture from file", false);

    hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data\\Textures\\pinksquare.jpg", nullptr, pinkTexture.GetAddressOf());
    ONFAILHRLOG(hr, "Failed to create wic texture from file", false);

    hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data\\Textures\\seamless_pavement.jpg", nullptr, pavementTexture.GetAddressOf());
    ONFAILHRLOG(hr, "Failed to create wic texture from file", false);

    // Initialize constant buffer(s)
    hr = cb_vs_vertexshader.initialize(device.Get(), deviceContext.Get());
    ONFAILHRLOG(hr, "Failed to initialize cb_vs_vertexhader constant buffer.", false);

    hr = cb_ps_pixelshader.initialize(device.Get(), deviceContext.Get());
    ONFAILHRLOG(hr, "Failed to initialize cb_ps_pixelshader constant buffer.", false);

    camera.setPosition(0.0f, 0.0f, -2.0f);
    camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);
    return true;
}
