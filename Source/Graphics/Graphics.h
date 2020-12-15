#pragma once

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h> 
#include "AdapterReader.h"
#include "Shaders.h"
#include "RenderableGameObject3D.h"
#include "ForwardRenderer.h"
#include "Light.h"
#include "Camera3D.h"
#include "Camera2D.h"
#include "../Timer.h"
#include "Sprite.h"
#include "ImGUI/ImGUIW.h"

namespace NGraphics {

    //class GraphicsDescription {

    //};

    class Graphics {
    public:
        bool initialize(HWND hwnd, int width, int height);
        void renderFrame();
        // TODO: fix that we have direct access to 2D camera
        // but for 3D we have getter
        Camera3D& const getCamera3D();
        Camera2D camera2D;
        Sprite sprite;
        RenderableGameObject3D nanosuit;
        Light light;
        void onWindowResize(UINT width, UINT height);
        bool isInitialized();

    private:
        bool initializeDirectX(HWND hwnd);
        bool initializeShaders();
        bool initializeScene();
        void createDeviceAndSwapChain(HWND hwnd);
        void createDepthStencilBufferAndView();
        void updateCamera();
        void createDepthStencilStates();
        void createAndSetViewport();
        float getAspectRatio();

        ForwardRenderer fRenderer;
        std::vector<IRenderable*> renderableGameObjects;

        Microsoft::WRL::ComPtr<ID3D11Device> device;
        std::wstring deviceDescription;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
        Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

        VertexShader vertexShader;
        VertexShader vertexShader_2d;
        PixelShader pixelShader;
        PixelShader pixelShader_2d;
        PixelShader pixelShader_2d_discard;
        
        ConstantBuffer<CB_VS_vertexshader_2d> cb_vs_vertexshader_2d;
        ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
        
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState_drawMask;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState_applyMask;

        Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStateCullFront;
        Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

        std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
        std::unique_ptr<DirectX::SpriteFont> spriteFont;
        size_t fontSize = 16;
        size_t spaceSize = 4;

        Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pavementTexture;

        int windowWidth = 800;
        int windowHeight = 600;
        float yFov = 45;
        float nearZ = 0.1;
        float farZ = 3000;
        bool initialized = false;
        Timer fpsTimer;
        Camera3D camera3D;
    };
}