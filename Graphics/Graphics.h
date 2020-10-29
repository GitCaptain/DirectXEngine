#pragma once

#include "AdapterReader.h"
#include "Shaders.h"
#include "Vertex.h"

class Graphics {
public:
    bool initialize(HWND hwnd, int width, int height);
    void renderFrame();
private:
    bool initializeDirectX(HWND hwnd, int width, int height);
    bool initializeShaders();
    bool initializeScene();

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

    VertexShader vertexShader;
    PixelShader pixelShader;
    
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
};