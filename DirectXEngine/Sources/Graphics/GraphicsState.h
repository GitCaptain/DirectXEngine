#pragma once

#include <d3d11.h>

struct GraphicsState {
    int* windowWidth = nullptr;
    int* windowHeight = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
};
