#pragma once

#include <wrl/client.h>
#include <vector>
#include <d3d11.h>
#include "interfaces/IRenderable.h"
#include "ConstantBuffer.h"
#include "Shaders.h"

static constexpr int buffer_cnt = 3;

class GBuffer final {
public:
    GBuffer() = default;
    ~GBuffer() = default;
    
    enum class positions { POSITION = 0, NORMAL = 1, COLOR = 2 };

    bool initialize(ID3D11Device* device, int width, int height);

    // position, normal, color
    Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetTextures[buffer_cnt];
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetViews[buffer_cnt];
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[buffer_cnt];

private:
    bool initTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D> &texture, 
                     ID3D11Device* device, 
                     int width, int height);

    bool initRenderTargetView(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> &rtv,
                              Microsoft::WRL::ComPtr<ID3D11Texture2D> &texture,
                              ID3D11Device* device);

    bool initShaderResourceView(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv, 
                                Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture, 
                                ID3D11Device* device);

};


class DeferredRenderer final {

public:

    DeferredRenderer() = default;
    ~DeferredRenderer();
    bool initialize(ID3D11Device* device,
                    ID3D11DeviceContext* deviceContext,
                    IDXGISwapChain* swapChain,
                    ID3D11RenderTargetView* renderTargetView, 
                    int width, int height);
    void renderFrame(const DirectX::XMMATRIX& viewProjectionMatrix);

    void setObjects(std::vector<IRenderable*>& renderables);

private:

    bool initializeShaders();

    VertexShader vsGeometryPass;
    VertexShader vsLightPass;
    PixelShader psGeometryPass;
    PixelShader psLightPass;

    GBuffer gBuffer;

    ConstantBuffer<CB_VS_vertexshader_2d> cb_vs_vertexshader_2d;
    ConstantBuffer<CB_VS_m_world_viewprojeciton> cb_vs_vertexshader;

    void renderPass(const DirectX::XMMATRIX& viewProjectionMatrix);

    std::vector<IRenderable*>* renderableObjects;

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    ID3D11DeviceContext* deviceContext = nullptr;
};
