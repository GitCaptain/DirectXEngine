#include <vector>
#include "Renderer.h"

/**
 * STATIC METHODS
 */

#pragma region DetermineShaderPath
const std::wstring Renderer::shaderFolder =
#ifdef _DEBUG
#ifdef _WIN64
L"x64\\Debug\\";
#else
L"x86\\Debug\\";
#endif
#else
#ifdef _WIN64
L"x64\\Release\\";
#else
L"x86\\Release\\";
#endif
#endif
#pragma endregion

void Renderer::createTexture(
    ID3D11Device* device,
    ID3D11Texture2D** pptexture,
    int Width,
    int Height,
    int textureBindFlags,
    DXGI_FORMAT textureFormat,
    size_t textureMipLevels
) {
    CD3D11_TEXTURE2D_DESC textureDesc(textureFormat, Width, Height);
    textureDesc.BindFlags = textureBindFlags;
    textureDesc.MipLevels = textureMipLevels;
    createTexture(device, textureDesc, pptexture);
}

void Renderer::createTexture(ID3D11Device* device, const CD3D11_TEXTURE2D_DESC& desc, ID3D11Texture2D** pptexture) {
    HRESULT hr = device->CreateTexture2D(&desc, nullptr, pptexture);
    COM_ERROR_IF_FAILED(hr, "Failed to create 2D texture.");
}

void Renderer::createRenderTargetView(
    ID3D11Device* device,
    ID3D11Resource* pResource,
    ID3D11RenderTargetView** ppRTView,
    const D3D11_RENDER_TARGET_VIEW_DESC* RTVDesc
) {
    HRESULT hr = device->CreateRenderTargetView(pResource, RTVDesc, ppRTView);
    COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");
}

void Renderer::createShaderResourceView(
    ID3D11Device* device,
    ID3D11Resource* pResource,
    ID3D11ShaderResourceView** ppSRView,
    const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc
) {
    HRESULT hr = device->CreateShaderResourceView(pResource, pDesc, ppSRView);
    COM_ERROR_IF_FAILED(hr, "Failed to create shader resource view.");
}

/**
 * PUBLIC INTERFACE
*/

bool Renderer::initRenderer(HWND renderWindowHandle, int windowWidth, int windowHeight) {

    this->renderWindowHandle = renderWindowHandle;
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;

    try {
        createDeviceAndSwapChain();
        createBackBufferRenderTargets();
        createDepthStencilState();
        createViewPort();
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

GraphicsState& Renderer::getGraphicsState() {
    return graphicsState;
}

void Renderer::preparePipeline() {
    deviceContext->RSSetViewports(1, &viewport);
}

void Renderer::present(size_t syncInterval, size_t flags) {
    swapChain->Present(syncInterval, flags);
    deviceContext->ClearState();
}

/**
 * PROTECTED INTERFACE
*/

AdapterData Renderer::selectAdapter() {

    std::vector<AdapterData> adapters = AdapterReader::getAdapters();
    if (adapters.empty()) {
        throw AdapterException("No DXGI Adapters found.");
    }

    // take adapter with maximum video memory
    AdapterData* bestAdapter = &adapters[0];
    for (AdapterData& adp : adapters) {
        if (adp.description.DedicatedVideoMemory > bestAdapter->description.DedicatedVideoMemory) {
            bestAdapter = &adp;
        }
    }

    return *bestAdapter;
}

void Renderer::createDepthStencilBuffer() {
    createTexture(device.Get(), &depthStencilBuffer, windowWidth, windowHeight, D3D11_BIND_DEPTH_STENCIL, DXGI_FORMAT_D24_UNORM_S8_UINT);
    HRESULT hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, &depthStencilView);
    COM_ERROR_IF_FAILED(hr, "Failed to create depth Stencil view.");
}

void Renderer::createDeviceAndSwapChain() {
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

void Renderer::createBackBufferRenderTargets() {
    ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), static_cast<void**>(&backBuffer));
    COM_ERROR_IF_FAILED(hr, "swapChain GetBuffer failed.");

    createRenderTargetView(device.Get(), backBuffer.Get(), &renderTargetView);
    createDepthStencilBuffer();
}

void Renderer::createDepthStencilState() {
    // Create depth stencil state
    CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

    HRESULT hr = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
    COM_ERROR_IF_FAILED(hr, "Failed to create depth Stencil view.");
}

void Renderer::createViewPort() {
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(windowWidth);
    viewport.Height = static_cast<float>(windowHeight);
}

void Renderer::createRasterizerState() {
    // Create rasterizer state
    CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
    HRESULT hr = device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");
}

void Renderer::createBlendState() {
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

void Renderer::createSamplerState() {
    // Create sampler description for sampler state
    CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    // Create sampler state
    HRESULT hr = device->CreateSamplerState(&sampDesc, &samplerState);
    COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
}

void Renderer::fillGraphicsState() {
    graphicsState.windowHeight = windowHeight;
    graphicsState.windowWidth = windowWidth;
    graphicsState.device = device.Get();
    graphicsState.deviceContext = deviceContext.Get();
}

bool Renderer::initConstantBuffers() {

    // initialize constants buffers
    try {
        HRESULT hr = cb_vs_vertexshader.initialize(device.Get(), deviceContext.Get());
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_vs_vertexhader constant buffer.");

        hr = cb_ps_ambientlight.initialize(device.Get(), deviceContext.Get());
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_ps_ambientlight constant buffer.");

        hr = cb_ps_pointlight.initialize(device.Get(), deviceContext.Get());
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_ps_pointlight constant buffer.");

        hr = cb_ps_camera.initialize(device.Get(), deviceContext.Get());
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_ps_camera constant buffer.");

        hr = cb_ps_lightsCount.initialize(device.Get(), deviceContext.Get());
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_ps_lightsCount constant buffer.");
    }
    catch (const COMException& e) {
        ErrorLogger::log(e);
        return false;
    }

    return true;
}

void Renderer::draw(const std::vector<const RenderableGameObject*>& renderables, const XMMATRIX& viewProj) {
    for (const RenderableGameObject* rgo : renderables) {
        const XMMATRIX& worldMatrix = rgo->getWorldMatrix();
        const XMMATRIX worldViewProjectionMatrix = worldMatrix * viewProj;
        const Model& model = rgo->getModel();
        const std::vector<Mesh>& meshes = model.getMeshes();
        for (const Mesh& mesh : meshes) {
            const VertexBuffer<Vertex3D>& vertexBuffer = mesh.getVertexBuffer();
            const IndexBuffer& indexBuffer = mesh.getIndexBuffer();
            const UINT offset = 0;
            const Texture* const t = mesh.getTexture();
            if (t) {
                deviceContext->PSSetShaderResources(0, 1, t->getTextureResourceViewAddress());
            }
            cb_vs_vertexshader.data.worldViewProjectionMatrix = mesh.getTransformMatrix() * worldViewProjectionMatrix;
            cb_vs_vertexshader.data.worldMatrix = mesh.getTransformMatrix() * worldMatrix;
            cb_vs_vertexshader.applyChanges();
            deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.getStridePtr(), &offset);
            deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
            deviceContext->DrawIndexed(indexBuffer.getIndexCount(), 0, 0);
        }
    }
}

void Renderer::prepareLights(const LightInfo& light, int slot) {

    cb_ps_ambientlight.data.color = light.ambient.lightColor;
    cb_ps_ambientlight.data.strength = light.ambient.lightStrength;
    cb_ps_ambientlight.applyChanges();

    cb_ps_lightsCount.data.lightCount = light.getLightsCnt();
    cb_ps_lightsCount.applyChanges();

    deviceContext->PSSetShaderResources(slot, 1, light.getLightTextureSRV(graphicsState));
}
