#pragma once

#include <DirectXMath.h>


/*
    Note: Constant buffers must be 16 bytes aligned
*/


struct CB_VS_m_world_viewprojeciton {
    DirectX::XMMATRIX viewProjectionMatrix;
    DirectX::XMMATRIX worldMatrix;
};

struct CB_VS_vertexshader_2d {
    DirectX::XMMATRIX worldViewProjectionMatrix;
};


// if you change MAXIMUM_LIGHTS here
// you also have to change it in constant buffer 
// for your pixel shader and recompile it!
constexpr int MAXIMUM_LIGHTS = 16;

struct CB_PS_light_per_frame {
    DirectX::XMFLOAT3 dynamicLightColor[MAXIMUM_LIGHTS];
    float dynamicLightStrength[MAXIMUM_LIGHTS];

    DirectX::XMFLOAT3 dynamicLightPosition[MAXIMUM_LIGHTS];

    DirectX::XMFLOAT3 ambientLightColor;
    float ambientLightStrength;

    float dynamicLightAttenuation_a; // make this variables
    float dynamicLightAttenuation_b; // common for all lights for now
    float dynamicLightAttenuation_c; // maybe changed in future
    
    int lightsCnt;
};

struct CB_PS_light_per_obj {
    bool lightSource;
};