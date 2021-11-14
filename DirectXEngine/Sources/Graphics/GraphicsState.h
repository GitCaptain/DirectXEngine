#pragma once

#include <d3d11.h>

struct GraphicsState {
    int windowWidth = 0;
    int windowHeight = 0;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
};
