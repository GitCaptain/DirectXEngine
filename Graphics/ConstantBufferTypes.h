#pragma once

#include <DirectXMath.h>


/*
Constant buffers must be 16 bytes aligned
*/


struct CB_VS_vertexshader {
    DirectX::XMMATRIX mat;
};

struct CB_PS_light {
    DirectX::XMFLOAT3 ambientLightColor;
    float ambientLightStrength;
};