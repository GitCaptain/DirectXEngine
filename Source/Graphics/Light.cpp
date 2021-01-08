#include "Light.h"
#include <wrl/client.h>

bool Light::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_m_world_viewprojeciton>& cb_vs_vertexshader) {
    
    if (!RenderableGameObject3D::initialize("Data/Objects/light.fbx", 
                                            device, 
                                            deviceContext, 
                                            cb_vs_vertexshader)) {
        return false;
    }

    if (!initLight(device)) {
        return false;
    }

    deviceContext->PSSetSamplers(10, 1, samplerStateLightInfo.GetAddressOf());
    deviceContext->PSSetShaderResources(10, 1, lightTexture.GetAddressOf());
    
    return true;
}

void Light::draw(const DirectX::XMMATRIX& viewProjectionMatrix) {
    for (int i = 0; i < lightsCnt; i++) {
        setPosition(lightData[2][i].x, lightData[2][i].y, lightData[2][i].z);
        RenderableGameObject3D::draw(viewProjectionMatrix);
    }
}


void Light::prepare(ID3D11DeviceContext* deviceContext) {
    updateLight(deviceContext);
}

bool Light::initLight(ID3D11Device* device) {

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

    // set attenuations to be greater than zero
    lightData[0][3].x = 0.01;
    lightData[0][3].y = 0.01;
    lightData[0][3].z = 0.01;

    // set initial ambient to be full white
    lightData[0][1].x = 1;
    lightData[0][1].y = 1;
    lightData[0][1].z = 1;
    lightData[0][2].x = 1;

    return true;
}

bool Light::updateLight(ID3D11DeviceContext* deviceContext) {

#ifdef ENABLE_IMGUI
    ImGui::Begin("Light Controls");
    ImGui::DragFloat3("Ambient light color", &lightData[0][1].x, 0.01, 0.0f, 1.0f);
    ImGui::DragFloat("Ambient light strength", &lightData[0][2].x, 0.01, 0.0f, 1.0f);
    ImGui::DragFloat("Dynamic light strength", &lightStrength, 0.01, 0.0f, 1.0f);
    ImGui::DragFloat3("Dynamic light Attenuations A, B, C", &lightData[0][3].x, 0.01, 0.01f, 1.0f);
    ImGui::InputInt("Light cnt", &lightsCnt);
    ImGui::End();
#endif

#ifdef _DEBUG
    std::string dls = std::to_string(lightStrength);
    dls = "dls: " + dls + "\n";
    OutputDebugStringA(dls.c_str());
#endif 

    if (lightsCnt < 0) {
        lightsCnt = 0;
    }

    lightData[0][0].x = MAX_LIGHT_CNT;
    lightData[0][0].y = lightsCnt;
    static double time = 0;
    for (int i = 0; i < lightsCnt; i++) {
        lightData[2][i] = { 10 * static_cast<float>(sin(time * i * i)),
                            10 * static_cast<float>(cos(time * i * i)),
                            0.0, 0.0 };
        lightData[1][i].x = lightColor.x;
        lightData[1][i].y = lightColor.y;
        lightData[1][i].z = lightColor.z;
        lightData[1][i].w = lightStrength;
    }
    time++;
    constexpr UINT mapFlags = 0;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(mappedResource));
    HRESULT hr = deviceContext->Map(lightInfo.Get(), 0, D3D11_MAP_WRITE_DISCARD, mapFlags, &mappedResource);
    ONFAILHRLOG(hr, "Failed to map light texture buffer.", false);
    CopyMemory(mappedResource.pData, &lightData, sizeof(lightData));
    deviceContext->Unmap(lightInfo.Get(), mapFlags);
    return true;
}