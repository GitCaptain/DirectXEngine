#pragma once
#include "RenderableGameObject.h"

class Light : public NGraphics::RenderableGameObject {
public:
    bool initialize(ID3D11Device *device, ID3D11DeviceContext *deviceContext, ConstantBuffer<CB_VS_vertexshader> &cb_vs_vertexshader);
    DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    float lightStrength = 1.0f;

    // ligth strength equation: strength = 1/(A + Bx +Cx*x)
    float attenuation_a = 1.0f;
    float attenuation_b = 0.1f;
    float attenuation_c = 0.1f;
};