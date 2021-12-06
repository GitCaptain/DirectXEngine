#include "Light.h"
#include "Render/Renderer.h"
#include <chrono>

bool LightInfo::initialize(GraphicsState& state, size_t max_light_cnt) {

    static_assert(sizeof(float) == 4 && "We assume that size of float is 4 bytes, when using 'DXGI_FORMAT_R32_FLOAT' texture format.");
    DXGI_FORMAT texFormat = DXGI_FORMAT_R32_FLOAT;
    const size_t texWidth = FLOATS_PER_DATA_ELEMENT * max_light_cnt;
    CD3D11_TEXTURE1D_DESC texDesc(texFormat, texWidth);
    texDesc.MipLevels = 1;
    texDesc.Usage = D3D11_USAGE_DYNAMIC;
    texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Renderer::createTexture(state.device, texDesc, &lightTex);

    CD3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc(D3D_SRV_DIMENSION_TEXTURE1D, texFormat);
    Renderer::createShaderResourceView(state.device, lightTex.Get(), &lightView, &SRVDesc);

    pointLights.reserve(max_light_cnt);
    lightData.reserve(texWidth);
    return true;
}

ID3D11ShaderResourceView* const* LightInfo::getLightTextureSRV(GraphicsState& state) const {

    auto insert_float_3 = [&lightData = lightData](const DirectX::XMFLOAT3& f3) {
        lightData.emplace_back(f3.x);
        lightData.emplace_back(f3.y);
        lightData.emplace_back(f3.z);
    };

    for(const PointLight& pl: pointLights) {
        insert_float_3(pl.lightColor);
        insert_float_3(pl.attenuations);
        insert_float_3(pl.getPositionFloat3());
        lightData.emplace_back(pl.lightStrength);
    }

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(mappedResource));
    HRESULT hr = state.deviceContext->Map(lightTex.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    ONFAILHRLOG(hr, "Failed to map light texture buffer.", nullptr);
    CopyMemory(mappedResource.pData, lightData.data(), lightData.size() * sizeof(float));
    state.deviceContext->Unmap(lightTex.Get(), 0);
    lightData.clear();
    return lightView.GetAddressOf();
}

int LightInfo::getLightsCnt() const noexcept {
    return pointLights.size();
}
