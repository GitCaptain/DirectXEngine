#pragma once

#include <d3d11.h>
#include "../GraphicsState.h"
#include "../AdapterReader.h"
#include "../../Scene/Scene.h"
#include "../Shaders.h"

class Renderer {

public:
    virtual bool initRenderer(HWND renderWindowHandle, int windowWidth, int windowHeight);
    virtual GraphicsState& getGraphicsState();
    virtual void preparePipeline();
    virtual void renderScene(const App::Scene *scene, const GraphicsSettings *gs, const float bgcolor[4]) = 0;
    virtual void present(size_t syncInterval, size_t flags);
    virtual ~Renderer() = default;

public:
    static void createTexture(
        ID3D11Device* device,
        ID3D11Texture2D** pptexture,
        int Width,
        int Height,
        int textureBindFlags,
        DXGI_FORMAT textureFormat = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
        size_t textureMipLevels = 1
    );

    static void createTexture(ID3D11Device* device, const CD3D11_TEXTURE2D_DESC& desc, ID3D11Texture2D** pptexture);
    static void createTexture(ID3D11Device* device, const CD3D11_TEXTURE1D_DESC& desc, ID3D11Texture1D** pptexture);

    static void createRenderTargetView(
        ID3D11Device* device,
        ID3D11Resource* pResource,
        ID3D11RenderTargetView** ppRTView,
        const D3D11_RENDER_TARGET_VIEW_DESC* RTVDesc = nullptr
    );


    static void createShaderResourceView(
        ID3D11Device* device,
        ID3D11Resource* pResource,
        ID3D11ShaderResourceView** ppSRView,
        const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc = nullptr
    );

protected:

    using RenderableGameObject = NGameObject::RenderableGameObject;
    using Model = NModel::Model;
    using XMMATRIX = DirectX::XMMATRIX;

    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    const static std::wstring shaderFolder;

    virtual AdapterData selectAdapter();
    virtual void createDeviceAndSwapChain();
    virtual void createDepthStencilBuffer();
    virtual void createBackBufferRenderTargets();
    virtual void createDepthStencilState();
    virtual void createViewPort();
    virtual void createRasterizerState();
    virtual void createBlendState();
    virtual void createSamplerState();
    virtual void fillGraphicsState();
    virtual bool initConstantBuffers();
    virtual void draw(
        const std::vector<const RenderableGameObject*>& renderables,
        const XMMATRIX &viewProj
    );
    virtual void prepareLights(const LightInfo& light, int slot);
    virtual bool initShaders() = 0;

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

    CD3D11_VIEWPORT viewport{};

    int windowWidth = 0;
    int windowHeight = 0;
    HWND renderWindowHandle = nullptr;

    GraphicsState graphicsState;

    ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader{};
    ConstantBuffer<CB_PS_PointLight> cb_ps_pointlight{};
    ConstantBuffer<CB_PS_Ambientlight> cb_ps_ambientlight{};
    ConstantBuffer<CB_PS_Camera> cb_ps_camera{};
    ConstantBuffer<CB_PS_LightsCount> cb_ps_lightsCount{};
    ConstantBuffer<CB_PS_GraphicsSettings> cb_ps_graphicsSettings{};
};
