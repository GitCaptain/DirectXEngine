#include "Light.h"
#include <wrl/client.h>

bool Light::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_m_world_viewprojeciton>& cb_vs_vertexshader) {
    
    if (!RenderableGameObject3D::initialize("Data/Objects/light.fbx", 
                                            device, 
                                            deviceContext, 
                                            cb_vs_vertexshader)) {
        return false;

    }
    
    auto hr = cb_ps_light.initialize(device, deviceContext);
    COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_ps_light constant buffer.");
    cb_ps_light.data.ambientLightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    cb_ps_light.data.ambientLightStrength = 1.0f;

    Microsoft::WRL::ComPtr<ID3D11Device> pd = device;
    std::wstring shaderFolder = L"x64\\Debug\\";
    if (!pixelShader_nolight.initialize(pd, shaderFolder + L"pixelShader_nolight.cso")) {
        return false;
    }

    return true;
}

void Light::draw(const DirectX::XMMATRIX& viewProjectionMatrix) {
#ifdef ENABLE_IMGUI
    // create imgui test window
    ImGui::Begin("Light Controls");
    ImGui::DragFloat3("Ambient light color", &cb_ps_light.data.ambientLightColor.x, 0.01, 0.0f, 1.0f);
    ImGui::DragFloat("Ambient light strength", &cb_ps_light.data.ambientLightStrength, 0.01, 0.0f, 1.0f);
    ImGui::DragFloat("Dynamic light Attenuation A", &attenuation_a, 0.01, 0.1f, 1.0f);
    ImGui::DragFloat("Dynamic light Attenuation B", &attenuation_b, 0.01, 0.0f, 1.0f);
    ImGui::DragFloat("Dynamic light Attenuation C", &attenuation_c, 0.01, 0.0f, 1.0f);
    ImGui::End();
#endif
    RenderableGameObject3D::draw(viewProjectionMatrix);
}


void Light::prepare(ID3D11DeviceContext* deviceContext) {
    cb_ps_light.data.dynamicLightColor = lightColor;
    cb_ps_light.data.dynamicLightStrength = lightStrength;
    cb_ps_light.data.dynamicLightPosition = pos;
    cb_ps_light.data.dynamicLightAttenuation_a = attenuation_a;
    cb_ps_light.data.dynamicLightAttenuation_b = attenuation_b;
    cb_ps_light.data.dynamicLightAttenuation_c = attenuation_c;
    cb_ps_light.applyChanges();

    deviceContext->PSSetConstantBuffers(0, 1, cb_ps_light.GetAddressOf());
    deviceContext->PSSetShader(pixelShader_nolight.getShader(), nullptr, 0);
}
