#pragma once

#include <DirectXMath.h>


/*
    Note: Constant buffers must be 16 bytes aligned
*/


struct CB_VS_vertexshader {
    DirectX::XMMATRIX worldViewProjectionMatrix;
    DirectX::XMMATRIX worldMatrix;
};

struct CB_VS_vertexshader_2d {
    DirectX::XMMATRIX worldViewProjectionMatrix;
};

struct CB_PS_Ambientlight {
    DirectX::XMFLOAT3 color;
    float strength;
};

struct CB_PS_LightsCount {
    int lightCount;
};

struct CB_PS_GraphicsSettings {
    float gamma;
};

struct CB_PS_PointLight {
    DirectX::XMFLOAT3 color;
    float strength;
    DirectX::XMFLOAT3 position;
    float specularStrength;
    DirectX::XMFLOAT3 attenuations;
    float shinessPower;
};

struct CB_PS_Camera {
    DirectX::XMFLOAT3 cameraWorldPosition;
};
