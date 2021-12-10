
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include "Texture.h"
#include "../ErrorLogger.h"

Texture::Texture(ID3D11Device* device, const Colors::Color& color, aiTextureType type, bool forceSRGB) {
    initialize1x1ColorTexture(device, color, type, forceSRGB);
}

Texture::Texture(
    ID3D11Device* device,
    const Colors::Color* colorData,
    UINT width,
    UINT height,
    aiTextureType type,
    bool forceSRGB
) {
    initializeColorTexture(device, colorData, width, height, type, forceSRGB);
}

Texture::Texture(ID3D11Device* device, const std::string& filePath, aiTextureType type, bool forceSRGB) {

    this->type = type;
    HRESULT hr;
    const std::wstring &&filename = StringHelper::stringToWide(filePath);

    if (StringHelper::getfileExtension(filePath) == "dds") {
        hr = DirectX::CreateDDSTextureFromFileEx(
            device,
            filename.c_str(),
            0,
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_SHADER_RESOURCE,
            0,
            0,
            forceSRGB,
            &texture,
            &diffuseTextureView
        );
    }
    else {
        hr = DirectX::CreateWICTextureFromFileEx(
            device,
            filename.c_str(),
            0,
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_SHADER_RESOURCE,
            0,
            0,
            forceSRGB ? DirectX::WIC_LOADER_FORCE_SRGB : DirectX::WIC_LOADER_DEFAULT,
            &texture,
            &diffuseTextureView
        );
    }
    if (FAILED(hr)) {
        initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type, forceSRGB);
    }
    return;
}

Texture::Texture(ID3D11Device* device, const uint8_t* pData, size_t size, aiTextureType type, bool forceSRGB) {
    this->type = type;
    HRESULT hr = DirectX::CreateWICTextureFromMemoryEx(
        device,
        pData,
        size,
        0,
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_SHADER_RESOURCE,
        0,
        0,
        forceSRGB? DirectX::WIC_LOADER_FORCE_SRGB: DirectX::WIC_LOADER_DEFAULT,
        texture.GetAddressOf(),
        diffuseTextureView.GetAddressOf()
    );
    COM_ERROR_IF_FAILED(hr, "Failed to create Texture from memory");
}

const aiTextureType Texture::getType() const {
    return type;
}

ID3D11ShaderResourceView* const  Texture::getTextureResourceView() const {
    return diffuseTextureView.Get();
}

ID3D11ShaderResourceView* const* Texture::getTextureResourceViewAddress() const {
    return diffuseTextureView.GetAddressOf();
}

void Texture::initialize1x1ColorTexture(
    ID3D11Device* device,
    const Colors::Color& colorData,
    aiTextureType type,
    bool srgb
) {
    initializeColorTexture(device, &colorData, 1, 1, type, srgb);
}

void Texture::initializeColorTexture(
    ID3D11Device* device,
    const Colors::Color* colorData,
    UINT width,
    UINT height,
    aiTextureType type_,
    bool srgb
) {
    type = type_;
    DXGI_FORMAT textureFormat = srgb? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: DXGI_FORMAT_R8G8B8A8_UNORM;
    CD3D11_TEXTURE2D_DESC textureDesc(textureFormat, width, height);
    ID3D11Texture2D *p2DTexture = nullptr;
    D3D11_SUBRESOURCE_DATA initialData{};
    initialData.pSysMem = colorData;
    initialData.SysMemPitch = width * sizeof(Colors::Color);
    HRESULT hr = device->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
    COM_ERROR_IF_FAILED(hr, "Failed to initialize texture from color data.");
    texture = p2DTexture;
    CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
    hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, diffuseTextureView.GetAddressOf());
    COM_ERROR_IF_FAILED(hr, "Failed to create shader resource view from texture generated from color data.");
}
