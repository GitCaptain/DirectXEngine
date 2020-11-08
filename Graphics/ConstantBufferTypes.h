#pragma once

#include <DirectXMath.h>


/*
    Note: Constant buffers must be 16 bytes aligned
*/


struct CB_VS_vertexshader {
    DirectX::XMMATRIX worldViewProjectionMatrix;
    DirectX::XMMATRIX worldMatrix;
};

struct CB_PS_light {
    DirectX::XMFLOAT3 ambientLightColor;
    float ambientLightStrength;

    DirectX::XMFLOAT3 dynamicLightColor;
    float dynamicLightStrength;

    DirectX::XMFLOAT3 dynamicLightPosition;
};