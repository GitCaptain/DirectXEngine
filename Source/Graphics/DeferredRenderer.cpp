#include "DeferredRenderer.h"

#define ENUM_AS_INT(x) ( \
           static_cast<int>(x) \
        )

DeferredRenderer::~DeferredRenderer() {
    // have to explicity detach ptr to default target,
    // otherwise it will be Release()'d and then destroyed
    // TODO: or not?
    device.Detach();
}

bool DeferredRenderer::initialize(ID3D11Device* device,
                                  ID3D11DeviceContext* deviceContext, 
                                  IDXGISwapChain* swapChain, 
                                  ID3D11RenderTargetView* renderTargetView,
                                  int width, int height) {
    this->device.Attach(device);
    this->deviceContext = deviceContext;
    
    return gBuffer.initialize(device, width, height);
}

void DeferredRenderer::renderFrame(const DirectX::XMMATRIX& viewProjectionMatrix) {

    float bgColor[] = { 0.0f, 0.0f, 0.0f, 1.0f};
    for (int i = 0; i < buffer_cnt; i++) {
        deviceContext->ClearRenderTargetView(gBuffer.renderTargetViews[i].Get(), bgColor);
    }

    deviceContext->VSSetShader(vsGeometryPass.getShader(), nullptr, 0);
    deviceContext->PSSetShader(psGeometryPass.getShader(), nullptr, 0);
    deviceContext->IASetInputLayout(vsGeometryPass.getInputLayout());

    // geometry pass
    renderPass(viewProjectionMatrix);
    
    // lightning pass 
    deviceContext->PSSetShaderResources(0, 1, gBuffer.shaderResourceViews[0].GetAddressOf());
    deviceContext->PSSetShaderResources(1, 1, gBuffer.shaderResourceViews[1].GetAddressOf());
    // apply constant buffer for lightning
    // set this buffer devcon->pssetconstbuf
    deviceContext->IASetInputLayout(vsLightPass.getInputLayout());
    deviceContext->VSSetShader(vsLightPass.getShader(), nullptr, 0);
    deviceContext->PSSetShader(psLightPass.getShader(), nullptr, 0);

    renderPass(viewProjectionMatrix);
}

void DeferredRenderer::setObjects(std::vector<IRenderable*>& renderables) {
    renderableObjects = &renderables;
}

bool DeferredRenderer::initializeShaders() {
    std::wstring shaderFolder = L"x64\\Debug\\";

    CD3D11_TEXTURE2D_DESC gBufferTextureDesc;

    // geometry pass shader
    D3D11_INPUT_ELEMENT_DESC VSFillGBuffer[] = {
        {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    
    UINT numElements = ARRAYSIZE(VSFillGBuffer);

    if (!vsGeometryPass.initialize(device, shaderFolder + L"VS_fill_gBuffer.cso",
                                   VSFillGBuffer, numElements)) {
        return false;
    }

    if (!psGeometryPass.initialize(device, shaderFolder + L"PS_fill_gBuffer.cso")) {
        return false;
    }

    // light pass shader
    D3D11_INPUT_ELEMENT_DESC VSUseGBuffer[] = {
        {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    numElements = ARRAYSIZE(VSUseGBuffer);

    if (!vsLightPass.initialize(device, shaderFolder + L"VS_use_gBuffer.cso",
        VSFillGBuffer, numElements)) {
        return false;
    }

    if (!psLightPass.initialize(device, shaderFolder + L"PS_use_gBuffer.cso")) {
        return false;
    }

    return true;
}

void DeferredRenderer::renderPass(const DirectX::XMMATRIX& viewProjectionMatrix) {
    for (auto& o : *renderableObjects) {
        o->prepare(deviceContext);
        o->draw(viewProjectionMatrix);
    }
}

bool GBuffer::initialize(ID3D11Device* device, int width, int height) {

    bool texturesInited = true;
    bool shaderResourcesInited = true;
    bool targetViewsInited = true;
    for (int i = 0; i < buffer_cnt; i++) {
        texturesInited &= initTexture(renderTargetTextures[i], device, width, height);
        targetViewsInited &= initRenderTargetView(renderTargetViews[i], renderTargetTextures[i], device);
        shaderResourcesInited &= initShaderResourceView(shaderResourceViews[i], renderTargetTextures[i], device);
    }

    return texturesInited && shaderResourcesInited && targetViewsInited;
}

bool GBuffer::initTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture, ID3D11Device* device, int width, int height){
    
    CD3D11_TEXTURE2D_DESC texDesc(DXGI_FORMAT_R32G32B32A32_FLOAT, width, height);
    texDesc.MipLevels = 1;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, texture.GetAddressOf());
    ONFAILHRLOG(hr, "Failed to create texture 2D for gBuffer.", false);
    
    return true;
}

bool GBuffer::initRenderTargetView(Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv,
                                   Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture,
                                   ID3D11Device* device) {

    CD3D11_RENDER_TARGET_VIEW_DESC rtvd;
    ZeroMemory(&rtvd, sizeof(rtvd));
    rtvd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvd.Texture2D.MipSlice = 0;

    HRESULT hr = device->CreateRenderTargetView(texture.Get(), &rtvd, rtv.GetAddressOf());
    ONFAILHRLOG(hr, "Failed to create render target view for gBuffer.", false);
    
    return true;
}

bool GBuffer::initShaderResourceView(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv, 
                                     Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture,
                                     ID3D11Device* device) {
    
    CD3D11_SHADER_RESOURCE_VIEW_DESC srvd(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32G32B32A32_FLOAT);

    HRESULT hr = device->CreateShaderResourceView(texture.Get(), &srvd, srv.GetAddressOf());
    ONFAILHRLOG(hr, "Failed to create shader resi", false);
   
    return true;
}

