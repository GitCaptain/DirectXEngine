#pragma once

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h> 
#include "AdapterReader.h"
#include "Sprite.h"
#include "GraphicsState.h"
#include "../Timer.h"
#include "../Scene/PongScene.h"
#include "ImGUIWrapper.h"

class Graphics {
public:
    bool initialize(HWND hwnd, int width, int height);
    void renderFrame();
    void update(HID::Keyboard& kbd, HID::Mouse& mouse, float dt);
    const GraphicsState& getGraphicsState() const;

private:
    bool initializeDirectX(HWND hwnd);
    bool initializeShaders();
    bool initializeScene();

    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStateCullFront;
    Microsoft::WRL::ComPtr<ID3D11BlendState> blendState; 

    std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
    std::unique_ptr<DirectX::SpriteFont> spriteFont;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

    Timer fpsTimer;
    int windowWidth = 800;
    int windowHeight = 600;
    Microsoft::WRL::ComPtr<ID3D11Device> device = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext = nullptr;
    GraphicsState state;
    App::PongScene renderScene;
    ImGUIW *imgui;
};
