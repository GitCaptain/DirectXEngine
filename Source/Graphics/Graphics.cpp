#include "Graphics.h"
#include <math.h>

#define SaveResetCOM(buffer) { \
            if ((buffer).Get() != nullptr){ \
                (buffer).Reset(); \
            } \
        }

template<typename T>
void constraintInPlace(T &a, const T& min_v, const T& max_v) {
    if (a < min_v) {
        a = min_v;
    }
    if (a > max_v) {
        a = max_v;
    }
}

using namespace NGraphics;

bool Graphics::initialize(HWND hwnd, int width, int height) {

    windowHeight = height;
    windowWidth = width;
    fpsTimer.startTimer();
    globalTimer.startTimer();
    
    if (!initializeDirectX(hwnd)) {
        return false;
    }

    if (!initializeShaders()) {
        return false;
    }

    if (!initializeScene()) {
        return false;
    }

    if (!fRenderer.initialize(deviceContext.Get())) {
        return false;
    }
    
    fRenderer.setObjects(renderableGameObjects);
    //renderableGameObjects.push_back(&nanosuit);
    // TODO: light set its own shader, 
    // so now we have to draw it last
    // need to remove this dependency
    // now it's not critical, because we have same ps shaders 
    // for nanosuit and light
    renderableGameObjects.push_back(&light);

#ifdef ENABLE_IMGUI
    // setup ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());
    ImGui::StyleColorsDark();
#endif

    initialized = true;
    return true;
}

void Graphics::renderFrame() {

#ifdef ENABLE_IMGUI
    // start dear imgui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif

    float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView.Get(), bgcolor);
    deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->RSSetState(rasterizerState.Get());
    deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
    deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    deviceContext->PSSetSamplers(0, 1, samplerStateTextures.GetAddressOf());

    deviceContext->PSSetSamplers(10, 1, samplerStateLightInfo.GetAddressOf());

    // sprite mask 
    //deviceContext->OMSetDepthStencilState(depthStencilState_drawMask.Get(), 0);
    //deviceContext->IASetInputLayout(vertexShader_2d.getInputLayout());
    //deviceContext->PSSetShader(pixelShader_2d_discard.getShader(), nullptr, 0); // we don't want to draw our mask
    //deviceContext->VSSetShader(vertexShader_2d.getShader(), nullptr, 0);
    //sprite.draw(camera2D.getWorldMatrix() * camera2D.getOrthoMatrix());

    deviceContext->VSSetShader(vertexShader.getShader(), nullptr, 0);
    deviceContext->PSSetShader(pixelShader_nolight.getShader(), nullptr, 0);
    deviceContext->IASetInputLayout(vertexShader.getInputLayout());
    //deviceContext->PSSetConstantBuffers(0, 1, cb_ps_light_frame.GetAddressOf());
    deviceContext->PSSetConstantBuffers(1, 1, cb_ps_light_obj.GetAddressOf());
    //deviceContext->OMSetDepthStencilState(depthStencilState_applyMask.Get(), 0);

    deviceContext->PSSetShaderResources(10, 1, lightTexture.GetAddressOf());

#ifdef ENABLE_IMGUI
    // create imgui test window
    ImGui::Begin("Video controls");
    ImGui::DragFloat("fov", &yFov, 1, 1.0f, 180.0f);
    ImGui::DragFloat("nearZ", &nearZ, 0.1, 0.1f, 10);
    ImGui::DragFloat("farZ", &farZ, 10, 10, 50000.0f);
    ImGui::End();
#endif

#pragma region old_light_work
//#ifdef ENABLE_IMGUI
//    ImGui::Begin("Light Controls");
//    ImGui::DragFloat3("Ambient light color", &cb_ps_light_frame.data.ambientLightColor.x, 0.01, 0.0f, 1.0f);
//    ImGui::DragFloat("Ambient light strength", &cb_ps_light_frame.data.ambientLightStrength, 0.01, 0.0f, 1.0f);
//    ImGui::DragFloat("Dynamic light Attenuation A", &light.attenuation_a, 0.01, 0.1f, 1.0f);
//    ImGui::DragFloat("Dynamic light Attenuation B", &light.attenuation_b, 0.01, 0.0f, 1.0f);
//    ImGui::DragFloat("Dynamic light Attenuation C", &light.attenuation_c, 0.01, 0.0f, 1.0f);
//    ImGui::InputInt("Light cnt", &lightsCnt);
//    ImGui::End();
//#endif
//
//    constraintInPlace(lightsCnt, 0, MAX_LIGHT_CNT);
//   
    //cb_ps_light_obj.data.lightSource = true;
    //cb_ps_light_obj.applyChanges();

    //cb_ps_light_frame.data.dynamicLightAttenuation_a = light.attenuation_a;
    //cb_ps_light_frame.data.dynamicLightAttenuation_b = light.attenuation_b;
    //cb_ps_light_frame.data.dynamicLightAttenuation_c = light.attenuation_c;
    //cb_ps_light_frame.data.lightsCnt = lightsCnt;
    //std::vector<XMFLOAT3> lightPos(lightsCnt);
    //double time = globalTimer.getMillisecondsElapsed() / 10000;
    //for (int i = 0; i < lightsCnt; i++) {
    //    lightPos[i] = { 10 * static_cast<float>(sin(time * i * i)),
    //                    10 * static_cast<float>(cos(time * i * i)),
    //                    0 };
    //    //XMFLOAT3 lightPos = { i * static_cast<float>(sin(time * i)),
    //    //                      i * static_cast<float>(cos(time * i)),
    //    //                      i * static_cast<float>(time) };
    //    cb_ps_light_frame.data.dynamicLightPosition[i] = lightPos[i];
    //    cb_ps_light_frame.data.dynamicLightColor[i] = light.lightColor;
    //    cb_ps_light_frame.data.dynamicLightStrength[i] = light.lightStrength;
    //}
    //cb_ps_light_frame.applyChanges();


    //XMMATRIX wvp = camera3D.getViewMatrix() * camera3D.getProjectionMatrix();
    //for (auto& pos : lightPos) {
    //    light.setPosition(pos);
    //    fRenderer.renderFrame(wvp);
    //}

    //cb_ps_light_obj.data.lightSource = false;
    //cb_ps_light_obj.applyChanges();
    //nanosuit.draw(wvp);
#pragma endregion

    XMMATRIX wvp = camera3D.getViewMatrix() * camera3D.getProjectionMatrix();
    cb_ps_light_obj.data.lightSource = true;
    cb_ps_light_obj.applyChanges();
    updateLight();
    for (int i = 0; i < lightsCnt; i++) {
        light.setPosition(lightData[2][i].x, lightData[2][i].y, lightData[2][i].z);
        fRenderer.renderFrame(wvp);
    }
    cb_ps_light_obj.data.lightSource = false;
    cb_ps_light_obj.applyChanges();
    nanosuit.draw(wvp);

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
    spriteFont->DrawString(spriteBatch.get(),
                           fpsString.c_str(),
                           DirectX::XMFLOAT2(0, 0),
                           DirectX::Colors::White, 
                           0.0f,
                           DirectX::XMFLOAT2(0.0f, 0.0f),
                           DirectX::XMFLOAT2(1.0f, 1.0f));
    spriteFont->DrawString(spriteBatch.get(),
                           deviceDescription.c_str(),
                           DirectX::XMFLOAT2(0, fontSize + spaceSize),
                           DirectX::Colors::White,
                           0.0f,
                           DirectX::XMFLOAT2(0.0f, 0.0f),
                           DirectX::XMFLOAT2(1.0f, 1.0f));
    spriteBatch->End();

#ifdef ENABLE_IMGUI
    // Assemble Together Draw data
    ImGui::Render();
    // render draw data
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

    const UINT vsync = 0;
    updateCamera();
    swapChain->Present(vsync, 0);
}

Camera3D& const Graphics::getCamera3D() {
    return camera3D;
}

void Graphics::onWindowResize(UINT width, UINT height) {

    if (!initialized) {
        return;
    }

    SaveResetCOM(renderTargetView);
    SaveResetCOM(depthStencilView);
    SaveResetCOM(depthStencilBuffer);

    windowWidth = width;
    windowHeight = height;
    
    // update swapChain and renderTargetView directly
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChain.Get()->GetDesc(&swapChainDesc);
    swapChain->ResizeBuffers(swapChainDesc.BufferCount,
                             width, height,
                             swapChainDesc.BufferDesc.Format,
                             swapChainDesc.Flags);
    
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    COM_ERROR_IF_FAILED(hr, "GetBuffer failed.");

    hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.GetAddressOf());
    ONFAILHRLOG(hr, "Can't create render target view for resized ");
    backBuffer.Reset();

    // update other resources through functions
    createDepthStencilBufferAndView();

    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

    createAndSetViewport();
    updateCamera();
}

bool Graphics::isInitialized() {
    return initialized;
}

bool Graphics::initializeDirectX(HWND hwnd) {
    try {

        createDeviceAndSwapChain(hwnd);
        
        createDepthStencilBufferAndView();

        createDepthStencilStates();

        createAndSetViewport();

        // Create rasterizer state
        CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
        HRESULT hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
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
        hr = device->CreateSamplerState(&sampDesc, samplerStateTextures.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state for textures.");

        deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

        initLight();
    }
    catch (const COMException& e) {
        ErrorLogger::log(e);
        return false;
    }
    return true;
}

bool Graphics::initializeShaders() {

    std::wstring shaderFolder = L"";

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

    if (!pixelShader_2d_discard.initialize(device, shaderFolder + L"pixelshader_2d_discard.cso")) {
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
        HRESULT hr = DirectX::CreateWICTextureFromFile(device.Get(), 
                                                       L"Data\\Textures\\seamless_grass.jpg",
                                                       nullptr, grassTexture.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file");

        hr = DirectX::CreateWICTextureFromFile(device.Get(), 
                                               L"Data\\Textures\\pinksquare.jpg", 
                                               nullptr, pinkTexture.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file");

        hr = DirectX::CreateWICTextureFromFile(device.Get(), 
                                               L"Data\\Textures\\seamless_pavement.jpg", 
                                               nullptr, pavementTexture.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file");
        
        // Initialize constant buffer(s)
        hr = cb_vs_vertexshader_2d.initialize(device.Get(), deviceContext.Get());
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_vs_vertexhader 2d constant buffer.");

        hr = cb_vs_vertexshader.initialize(device.Get(), deviceContext.Get());
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_vs_vertexhader constant buffer.");

        //hr = cb_ps_light_frame.initialize(device.Get(), deviceContext.Get());
        //COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_ps_light_frame constant buffer.");
        hr = cb_ps_light_obj.initialize(device.Get(), deviceContext.Get());
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_ps_light_obj constant buffer.");
        
        // TODO: move set initial values to another function
        //cb_ps_light_frame.data.ambientLightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
        //cb_ps_light_frame.data.ambientLightStrength = 1.0f;

        // Initialize Model(s)
        if(!nanosuit.initialize("Data\\Objects\\nanosuit\\nanosuit.obj", 
                                device.Get(), deviceContext.Get(), cb_vs_vertexshader)){
            return false;
        }

        if (!light.initialize(device.Get(), deviceContext.Get(), cb_vs_vertexshader)) {
            return false;
        }

        // TODO: FIX: when load anything from broken path,
        // the engine just crush silently, need to make the crush reason more clear
        if (!sprite.initialize(device.Get(), deviceContext.Get(), 256, 256, 
                               "Data/textures/circle.png", cb_vs_vertexshader_2d)) {
            return false;
        }

        sprite.setPosition(DirectX::XMFLOAT3(windowWidth/2 - sprite.getWidth()/2,
                                             windowHeight/2 - sprite.getHeight()/2, 0.0f));
        
        camera2D.setProjectionValues(windowWidth, windowHeight, 0.0f, 1.0f);

        nanosuit.setPosition(2.0f, 0.0f, 0.0f);

        camera3D.setPosition(0.0f, 0.0f, -2.0f);
        camera3D.setProjectionValues(yFov, getAspectRatio(), nearZ, farZ);
    }
    catch (const COMException& e) {
        ErrorLogger::log(e);
        return false;
    }
    return true;
}

bool NGraphics::Graphics::initLight() {

    // Refer to CB_light.hlsli 
    // to read texture description

    DXGI_FORMAT texFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;

    CD3D11_TEXTURE2D_DESC texDesc(texFormat, MAX_LIGHT_CNT, LIGHT_TEXTURE_HEIGHT);
    texDesc.MipLevels = 1;
    texDesc.Usage = D3D11_USAGE_DYNAMIC;
    texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, lightInfo.GetAddressOf());
    ONFAILHRLOG(hr, "Can't create texture for light info.", false);

    CD3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc(D3D_SRV_DIMENSION_TEXTURE2D, texFormat);
    hr = device->CreateShaderResourceView(lightInfo.Get(), &SRVDesc, lightTexture.GetAddressOf());
    ONFAILHRLOG(hr, "Can't create shader resource view for light texture", false);

    // Create sampler description for sampler state
    CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    // Create sampler state
    hr = device->CreateSamplerState(&sampDesc, samplerStateLightInfo.GetAddressOf());
    COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state for lights.");

    return true;
}

bool NGraphics::Graphics::updateLight() {

#ifdef ENABLE_IMGUI
    ImGui::Begin("Light Controls");
    ImGui::DragFloat3("Ambient light color", &lightData[0][1].x, 0.01, 0.0f, 1.0f);
    ImGui::DragFloat("Ambient light strength", &lightData[0][2].x, 0.01, 0.0f, 1.0f);
    ImGui::DragFloat("Dynamic light strength", &light.lightStrength, 0.01, 0.0f, 1.0f);
    ImGui::DragFloat("Dynamic light Attenuation A", &light.attenuation_a, 0.01, 0.1f, 1.0f);
    ImGui::DragFloat("Dynamic light Attenuation B", &light.attenuation_b, 0.01, 0.0f, 1.0f);
    ImGui::DragFloat("Dynamic light Attenuation C", &light.attenuation_c, 0.01, 0.0f, 1.0f);
    ImGui::InputInt("Light cnt", &lightsCnt);
    ImGui::End();
#endif

    std::wstring dls = std::to_wstring(light.lightStrength);
    dls = L"dls: " + dls + L"\n";
    OutputDebugStringW(dls.c_str());
    constraintInPlace(lightsCnt, 0, MAX_LIGHT_CNT);

    lightData[0][0].x = MAX_LIGHT_CNT;
    lightData[0][0].y = lightsCnt;
    lightData[0][3].x = light.attenuation_a;
    lightData[0][3].y = light.attenuation_b;
    lightData[0][3].z = light.attenuation_c;

    double time = globalTimer.getMillisecondsElapsed() / 10000;
    for (int i = 0; i < lightsCnt; i++) {
        lightData[2][i] = { 10 * static_cast<float>(sin(time * i * i)),
                            10 * static_cast<float>(cos(time * i * i)),
                            0.0, 0.0 };
        lightData[1][i].x = light.lightColor.x;
        lightData[1][i].y = light.lightColor.y;
        lightData[1][i].z = light.lightColor.z;
        lightData[1][i].w = light.lightStrength;
    }

    constexpr UINT mapFlags = 0;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(mappedResource));
    HRESULT hr = deviceContext->Map(lightInfo.Get(), 0, D3D11_MAP_WRITE_DISCARD, mapFlags, &mappedResource);
    ONFAILHRLOG(hr, "Failed to map light texture buffer.", false);
    CopyMemory(mappedResource.pData, &lightData, sizeof(lightData));
    deviceContext->Unmap(lightInfo.Get(), mapFlags);
    return true;
}

float Graphics::getAspectRatio() {
    return static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
}

void Graphics::createDeviceAndSwapChain(HWND hwnd) {

    auto adapters = AdapterReader::getAdapters();
    ONFAILLOG(adapters.empty(), "No DXGI Adapters found.");

    // take adapter with maximum video memory
    auto bestAdapter = &adapters[0];
    for (const auto& adp : adapters) {
        if (adp.description.DedicatedVideoMemory > bestAdapter->description.DedicatedVideoMemory) {
            bestAdapter = const_cast<AdapterData*>(&adp);
        }
    }

    deviceDescription = bestAdapter->description.Description;

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
    
    renderTargetView.Reset();
    hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.GetAddressOf());
    COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");
}

void Graphics::createDepthStencilBufferAndView() {
    
    // Describe our depth/stencil buffer
    CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, windowWidth, windowHeight);
    depthStencilTextureDesc.MipLevels = 1;
    depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    HRESULT hr = device->CreateTexture2D(&depthStencilTextureDesc, nullptr, depthStencilBuffer.GetAddressOf());
    COM_ERROR_IF_FAILED(hr, "Failed to create depth Stencil buffer.");

    hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, depthStencilView.GetAddressOf());
    COM_ERROR_IF_FAILED(hr, "Failed to create depth Stencil view.");
    
}

void Graphics::updateCamera() {
    camera3D.setProjectionValues(yFov, getAspectRatio(), nearZ, farZ);
}

void Graphics::createDepthStencilStates() {
    // Create depth stencil state
    CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
    
    depthStencilState.Reset();
    HRESULT hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
    COM_ERROR_IF_FAILED(hr, "Failed to create depth Stencil view.");

    //CD3D11_DEPTH_STENCIL_DESC depthStencilDesc_drawMask(D3D11_DEFAULT);
    //depthStencilDesc_drawMask.DepthEnable = FALSE;
    //depthStencilDesc_drawMask.StencilEnable = TRUE;

    //depthStencilDesc_drawMask.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
    //depthStencilDesc_drawMask.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
    //depthStencilDesc_drawMask.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
    //depthStencilDesc_drawMask.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;

    //depthStencilDesc_drawMask.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
    //depthStencilDesc_drawMask.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
    //depthStencilDesc_drawMask.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
    //depthStencilDesc_drawMask.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_INCR_SAT;
    //depthStencilState_drawMask.Reset();
    //hr = device->CreateDepthStencilState(&depthStencilDesc_drawMask, depthStencilState_drawMask.GetAddressOf());
    //COM_ERROR_IF_FAILED(hr, "Failed to create depth Stencil state for drawing mask.");

    //CD3D11_DEPTH_STENCIL_DESC depthStencilDesc_applyMask(D3D11_DEFAULT);
    //depthStencilDesc_applyMask.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
    //depthStencilDesc_applyMask.StencilEnable = TRUE;

    //depthStencilDesc_applyMask.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
    //depthStencilDesc_applyMask.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
    //depthStencilDesc_applyMask.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
    //depthStencilDesc_applyMask.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;

    //depthStencilDesc_applyMask.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
    //depthStencilDesc_applyMask.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
    //depthStencilDesc_applyMask.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
    //depthStencilDesc_applyMask.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
    //
    //depthStencilState_applyMask.Reset();
    //hr = device->CreateDepthStencilState(&depthStencilDesc_applyMask, depthStencilState_applyMask.GetAddressOf());
    //COM_ERROR_IF_FAILED(hr, "Failed to create depth Stencil state for applying mask.");
}

void Graphics::createAndSetViewport() {

    // create and set viewoport
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));
    deviceContext->RSSetViewports(1, &viewport);
}
