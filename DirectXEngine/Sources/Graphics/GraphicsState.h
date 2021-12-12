#pragma once

#include <d3d11.h>

struct GraphicsState {
    int windowWidth = 0;
    int windowHeight = 0;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
};

struct GraphicsSettings {
    float fov = 90.f;
    float nearZ = 0.1f;
    float farZ = 3000.f;
    float gammaCoef = 2.2f;
    float exposure = 0.0f;
    bool hdr = false;
};
