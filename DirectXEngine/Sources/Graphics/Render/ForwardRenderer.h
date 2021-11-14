#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include "Renderer.h"
#include "../AdapterReader.h"

class ForwardRenderer final: public Renderer {

    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    bool initRenderer(HWND renderWindowHandle, int windowWidth, int windowHeight) override;
    GraphicsState& getGraphicsState() override;
    void preparePipeline() override;
    void present(size_t syncInterval, size_t flags) override;

private:

    AdapterData selectAdapter();
    void createDeviceAndSwapChain();
    void createRenderTargetView();
    void createDepthStencilBuffer();
    void createAndSetRenderTargets();
    void createDepthStencilState();
    void createAndSetViewPort();
    void createRasterizerState();
    void createBlendState();
    void createSamplerState();
    void fillGraphicsState();

    ComPtr<IDXGISwapChain> swapChain = nullptr;
    ComPtr<ID3D11RenderTargetView> renderTargetView = nullptr;
    ComPtr<ID3D11DepthStencilView> depthStencilView = nullptr;
    ComPtr<ID3D11Texture2D> depthStencilBuffer = nullptr;
    ComPtr<ID3D11DepthStencilState> depthStencilState = nullptr;
    ComPtr<ID3D11RasterizerState> rasterizerState = nullptr;
    ComPtr<ID3D11BlendState> blendState = nullptr;
    ComPtr<ID3D11SamplerState> samplerState = nullptr;
    ComPtr<ID3D11Device> device = nullptr;
    ComPtr<ID3D11DeviceContext> deviceContext = nullptr;

    int windowWidth = 0;
    int windowHeight = 0;
    HWND renderWindowHandle;

    GraphicsState graphicsState;
};
