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

    cb_ps_light.data.dynamicLightColor = light.lightColor;
    cb_ps_light.data.dynamicLightStrength = light.lightStrength;
    cb_ps_light.data.dynamicLightPosition = light.getPositionFloat3();
    cb_ps_light.data.dynamicLightAttenuation_a = light.attenuation_a;
    cb_ps_light.data.dynamicLightAttenuation_b = light.attenuation_b;
    cb_ps_light.data.dynamicLightAttenuation_c = light.attenuation_c;

    cb_ps_light.applyChanges();
    deviceContext->PSSetConstantBuffers(0, 1, cb_ps_light.GetAddressOf());

    float bgcolor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    deviceContext->ClearRenderTargetView(renderTargetView.Get(), bgcolor);
    deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); 
    
    deviceContext->IASetInputLayout(vertexShader.getInputLayout());
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->RSSetState(rasterizerState.Get());
    deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
    deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
    deviceContext->VSSetShader(vertexShader.getShader(), nullptr, 0);
    deviceContext->PSSetShader(pixelShader.getShader(), nullptr, 0);
    
    { // Pavement cube
        gameObject.draw(camera3D.getViewMatrix() * camera3D.getProjectionMatrix());
    }

    { // TODO: take multiption out
        deviceContext->PSSetShader(pixelShader_nolight.getShader(), nullptr, 0);
        light.draw(camera3D.getViewMatrix() * camera3D.getProjectionMatrix());
    }

    deviceContext->IASetInputLayout(vertexShader_2d.getInputLayout());
    deviceContext->PSSetShader(pixelShader_2d.getShader(), nullptr, 0);
    deviceContext->VSSetShader(vertexShader_2d.getShader(), nullptr, 0);

    sprite.draw(camera2D.getWorldMatrix() * camera2D.getOrthoMatrix());

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
    ImGui::Begin("Light Controls");
    ImGui::DragFloat3("Ambient light color", &cb_ps_light.data.ambientLightColor.x, 0.01, 0.0f, 1.0f);
    ImGui::DragFloat("Ambient light strength", &cb_ps_light.data.ambientLightStrength, 0.01, 0.0f, 1.0f);
    ImGui::DragFloat("Dynamic light Attenuation A", &light.attenuation_a, 0.01, 0.1f, 1.0f);
    ImGui::DragFloat("Dynamic light Attenuation B", &light.attenuation_b, 0.01, 0.0f, 1.0f);
    ImGui::DragFloat("Dynamic light Attenuation C", &light.attenuation_c, 0.01, 0.0f, 1.0f);
    ImGui::End();
    // Assemble Together Draw data
    ImGui::Render();
    // render draw data
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

    const UINT vsync = 0;
    swapChain->Present(vsync, 0);
}

Camera::Camera3D& const Graphics::getCamera3D() {
    return camera3D;
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

        // create and set viewoport
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
        spriteFont = std::make_unique<DirectX::SpriteFont>(device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");

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

    // 2d shaders 
    D3D11_INPUT_ELEMENT_DESC layout2D[] = {
        {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    UINT numElements2D = ARRAYSIZE(layout2D);

    if (!vertexShader_2d.initialize(device, shaderFolder + L"vertexshader_2d.cso", layout2D, numElements2D)) {
        return false;
    }

    if (!pixelShader_2d.initialize(device, shaderFolder + L"pixelshader_2d.cso")) {
        return false;
    }
    // 3d shaders
    D3D11_INPUT_ELEMENT_DESC layout3D[] = {
        {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    UINT numElements = ARRAYSIZE(layout3D);

    if (!vertexShader.initialize(device, shaderFolder + L"vertexshader.cso", layout3D, numElements)) {
        return false;
    }

    if (!pixelShader.initialize(device, shaderFolder + L"pixelshader.cso")) {
        return false;
    }

    if (!pixelShader_nolight.initialize(device, shaderFolder + L"pixelShader_nolight.cso")) {
        return false;
    }

    return true;
}

bool Graphics::initializeScene() {
    try {
        // Load texture
        HRESULT hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data\\Textures\\seamless_grass.jpg", nullptr, grassTexture.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file");

        hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data\\Textures\\pinksquare.jpg", nullptr, pinkTexture.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file");

        hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data\\Textures\\seamless_pavement.jpg", nullptr, pavementTexture.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file");

        // Initialize constant buffer(s)
        hr = cb_vs_vertexshader_2d.initialize(device.Get(), deviceContext.Get());
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_vs_vertexhader 2d constant buffer.");

        hr = cb_vs_vertexshader.initialize(device.Get(), deviceContext.Get());
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_vs_vertexhader constant buffer.");

        hr = cb_ps_light.initialize(device.Get(), deviceContext.Get());
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_ps_light constant buffer.");

        cb_ps_light.data.ambientLightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
        cb_ps_light.data.ambientLightStrength = 1.0f;

        // Initialize Model(s)
        if(!gameObject.initialize("Data\\Objects\\nanosuit\\nanosuit.obj", device.Get(), deviceContext.Get(), cb_vs_vertexshader)){
            return false;
        }

        if (!light.initialize(device.Get(), deviceContext.Get(), cb_vs_vertexshader)) {
            return false;
        }

        if (!sprite.initialize(device.Get(), deviceContext.Get(), 256, 256, "Data/textures/sprite_256x256.png", cb_vs_vertexshader_2d)) {
            return false;
        }

        camera2D.setProjectionValues(windowWidth, windowHeight, 0.0f, 1.0f);

        gameObject.setPosition(2.0f, 0.0f, 0.0f);

        camera3D.setPosition(0.0f, 0.0f, -2.0f);
        camera3D.setProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 3000.0f);
    }
    catch (const COMException& e) {
        ErrorLogger::log(e);
        return false;
    }
    return true;
}
