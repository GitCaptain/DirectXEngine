#pragma once

struct GraphicsState {
    int* windowWidth;
    int* windowHeight;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
};
