#pragma once

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h> 
#include "AdapterReader.h"
#include "Shaders.h"
#include "Camera3D.h"
#include "../Timer.h"
#include "RenderableGameObject.h"
#include "Light.h"
#include "Camera2D.h"
#include "Sprite.h"

#ifdef ENABLE_IMGUI
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#endif

struct GraphicsState {
    int *windowWidth;
    int *windowHeight;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext *deviceContext = nullptr;
};

class Graphics {
public:
    bool initialize(HWND hwnd, int width, int height);
    void renderFrame();
    const GraphicsState& getGraphicsState() const;
    NGameObject::Camera3D& getCamera3D();
    NGameObject::RenderableGameObject nanoSuite;
    Light light;

private:
    bool initializeDirectX(HWND hwnd);
    bool initializeShaders();
    bool initializeScene();

    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

    VertexShader vertexShader;
    PixelShader pixelShader;
    PixelShader pixelShader_nolight;

    ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader; 
    ConstantBuffer<CB_PS_light> cb_ps_light; 
    
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStateCullFront;
    Microsoft::WRL::ComPtr<ID3D11BlendState> blendState; 

    std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
    std::unique_ptr<DirectX::SpriteFont> spriteFont;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pavementTexture;

    Timer fpsTimer;
    NGameObject::Camera3D camera3D;
    int windowWidth = 800;
    int windowHeight = 600;
    Microsoft::WRL::ComPtr<ID3D11Device> device = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext = nullptr;
    GraphicsState state;
};