#include "Light.h"
#include "Render/Renderer.h"
#include <iostream>

LightInfo::PointLightDesc::PointLightDesc(
    DirectX::XMFLOAT3 color,
    DirectX::XMFLOAT3 attenuations,
    DirectX::XMFLOAT3 pos,
    float strength
): lightColor(color), p1(0), attenuations(attenuations), p2(0), position(pos), lightStrength(strength)
{}

bool LightInfo::initialize(GraphicsState& state, size_t max_light_cnt) {
    constexpr size_t LIGHT_TEXTURE_WIDTH = 3;
    DXGI_FORMAT texFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
    CD3D11_TEXTURE2D_DESC texDesc(texFormat, LIGHT_TEXTURE_WIDTH, max_light_cnt);
    texDesc.MipLevels = 1;
    texDesc.Usage = D3D11_USAGE_DYNAMIC;
    texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Renderer::createTexture(state.device, texDesc, &lightTex);

    CD3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc(D3D_SRV_DIMENSION_TEXTURE2D, texFormat);
    Renderer::createShaderResourceView(state.device, lightTex.Get(), &lightView, &SRVDesc);

    pointLights.reserve(max_light_cnt);
    lightData.reserve(max_light_cnt);
    return true;
}

ID3D11ShaderResourceView* const* LightInfo::getLightTextureSRV(GraphicsState& state) const {
    for(const PointLight& pl: pointLights) {
        lightData.emplace_back(pl.lightColor, pl.attenuations, pl.getPositionFloat3(), pl.lightStrength);
    }

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(mappedResource));
    HRESULT hr = state.deviceContext->Map(lightTex.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    ONFAILHRLOG(hr, "Failed to map light texture buffer.", nullptr);
    std::cout << mappedResource.DepthPitch << ' ' << mappedResource.RowPitch << ' ' << mappedResource.pData << ' ' << lightTex.Get() << '\n';
    CopyMemory(mappedResource.pData, lightData.data(), lightData.size() * sizeof(lightData[0]));
    state.deviceContext->Unmap(lightTex.Get(), 0);
    lightData.clear();
    return lightView.GetAddressOf();
}

int LightInfo::getLightsCnt() const noexcept {
    return pointLights.size();
}
