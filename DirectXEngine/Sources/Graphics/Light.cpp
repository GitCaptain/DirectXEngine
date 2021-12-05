#include "Light.h"
#include "Render/Renderer.h"
#include <sstream>
#include <chrono>
#include <iostream>

#include "ScreenGrab.h"

//LightInfo::PointLightDesc::PointLightDesc(
//    DirectX::XMFLOAT3 color,
//    DirectX::XMFLOAT3 attenuations,
//    DirectX::XMFLOAT3 pos,
//    float strength
//): lightColor(color), p1(0), attenuations(attenuations), p2(0), position(pos), lightStrength(strength)
//{}

bool LightInfo::initialize(GraphicsState& state, size_t max_light_cnt) {
    constexpr size_t LIGHT_TEXTURE_WIDTH = 3;
    DXGI_FORMAT texFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
    CD3D11_TEXTURE1D_DESC texDesc(texFormat, LIGHT_TEXTURE_WIDTH * max_light_cnt);
    texDesc.MipLevels = 1;
    texDesc.Usage = D3D11_USAGE_DYNAMIC;
    texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Renderer::createTexture(state.device, texDesc, &lightTex);

    CD3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc(D3D_SRV_DIMENSION_TEXTURE1D, texFormat);
    Renderer::createShaderResourceView(state.device, lightTex.Get(), &lightView, &SRVDesc);

    pointLights.reserve(max_light_cnt);
    lightData.reserve(max_light_cnt);
    return true;
}

int saved = 0;

std::wstring xf3str(const DirectX::XMFLOAT3& xm) {
    std::wstringstream ss;
    ss << xm.x << ' ' << xm.y << ' ' << xm.z;
    return ss.str();
}

std::wstring PLDSTR(const PointLight& pl) {
    std::wstringstream ss;
    ss << xf3str(pl.lightColor) << ' ' << xf3str(pl.attenuations) << ' ' << xf3str(pl.getPositionFloat3()) << ' ' << pl.lightStrength;
    return ss.str();
}

#define EXPAND(X) X.x, X.y, X.z

ID3D11ShaderResourceView* const* LightInfo::getLightTextureSRV(GraphicsState& state) const {
    for(const PointLight& pl: pointLights) {
        lightData.emplace_back(EXPAND(pl.lightColor), EXPAND(pl.attenuations), EXPAND(pl.getPositionFloat3()), pl.lightStrength);
        OutputDebugStringW((PLDSTR(pl) + L'\n').c_str());
    }

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(mappedResource));
    HRESULT hr = state.deviceContext->Map(lightTex.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    //ONFAILHRLOG(hr, "Failed to map light texture buffer.", nullptr);
    //std::stringstream ss;
    //ss << mappedResource.DepthPitch << ' ' << mappedResource.RowPitch << ' ' << mappedResource.pData << ' ' << lightTex.Get() << '\n';
    //OutputDebugStringA(ss.str().c_str());
    auto sz = lightData.size() * sizeof(PointLightDesc);
    if (saved < 1000) {
        auto tm = std::chrono::high_resolution_clock::now().time_since_epoch();
        auto x = std::chrono::duration_cast<std::chrono::microseconds>(tm).count();
        DirectX::SaveDDSTextureToFile(state.deviceContext, lightTex.Get(), (L"Screenshots\\" + std::to_wstring(x) + L".DDS").c_str());
        saved++;
    }
    CopyMemory(mappedResource.pData, lightData.data(), sz);
    state.deviceContext->Unmap(lightTex.Get(), 0);
    lightData.clear();
    return lightView.GetAddressOf();
}

int LightInfo::getLightsCnt() const noexcept {
    return pointLights.size();
}
