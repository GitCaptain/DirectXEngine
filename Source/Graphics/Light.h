#pragma once

#include "RenderableGameObject3D.h"
#include "Shaders.h"
#include "ImGUI/ImGUIW.h"

class Light : public NGraphics::RenderableGameObject3D {
public:
    bool initialize(ID3D11Device *device, ID3D11DeviceContext *deviceContext, ConstantBuffer<CB_VS_m_world_viewprojeciton> &cb_vs_vertexshader);
    
    void prepare(ID3D11DeviceContext* deviceContext) override;
    void draw(const DirectX::XMMATRIX& viewProjectionMatrix) override;

    DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    float lightStrength = 1.0f;

    // ligth strength equation: strength = 1/(A + Bx +Cx*x)
    float attenuation_a = 1.0f;
    float attenuation_b = 0.1f;
    float attenuation_c = 0.1f;

private:
    ConstantBuffer<CB_PS_light> cb_ps_light;
    PixelShader pixelShader_nolight;
};