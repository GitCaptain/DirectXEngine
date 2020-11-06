
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include "Texture.h"
#include "../ErrorLogger.h"

Texture::Texture(ID3D11Device* device, const Colors::Color& color, aiTextureType type) {
    initialize1x1ColorTexture(device, color, type);
}

Texture::Texture(ID3D11Device* device, 
                 const Colors::Color* colorData,
                 UINT width, 
                 UINT height, 
                 aiTextureType type) {
    initializeColorTexture(device, colorData, width, height, type);
}

Texture::Texture(ID3D11Device* device, const std::string& filePath, aiTextureType type) {

    this->type = type;
    HRESULT hr;
    if (StringHelper::getfileExtension(filePath) == "dds") {
        hr = DirectX::CreateDDSTextureFromFile(device, StringHelper::stringToWide(filePath).c_str(), texture.GetAddressOf(), textureView.GetAddressOf());
    }
    else {
        hr = DirectX::CreateWICTextureFromFile(device, StringHelper::stringToWide(filePath).c_str(), texture.GetAddressOf(), textureView.GetAddressOf());
    }
    if (FAILED(hr)) {
        initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type);
    }
    return;
}

aiTextureType Texture::getType() {
    return type;
}

ID3D11ShaderResourceView* Texture::getTextureResourceView() {
    return textureView.Get();
}

ID3D11ShaderResourceView** Texture::getTextureResourceViewAddress() {
    return textureView.GetAddressOf();
}

void Texture::initialize1x1ColorTexture(ID3D11Device* device, 
                                        const Colors::Color& colorData, 
                                        aiTextureType type) {
    initializeColorTexture(device, &colorData, 1, 1, type);
}

void Texture::initializeColorTexture(ID3D11Device* device,
                                     const Colors::Color* colorData,
                                     UINT width,
                                     UINT height,
                                     aiTextureType type) {
    this->type = type;
    CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
    ID3D11Texture2D *p2DTexture = nullptr;
    D3D11_SUBRESOURCE_DATA initialData{};
    initialData.pSysMem = colorData;
    initialData.SysMemPitch = width * sizeof(Colors::Color);
    HRESULT hr = device->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
    COM_ERROR_IF_FAILED(hr, "Failed to initialize texture from color data.");
    texture = static_cast<ID3D11Texture2D*>(p2DTexture);
    CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
    hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, textureView.GetAddressOf());
    COM_ERROR_IF_FAILED(hr, "Failed to create shader resource view from texture generated from color data.");
}
