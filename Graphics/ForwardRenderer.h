#pragma once

#include <wrl/client.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <vector>
#include "interfaces/IRenderable.h"
#include <d3d11.h>
#include "ConstantBuffer.h"
#include "Shaders.h"

class ForwardRenderer {

public:

    ForwardRenderer() = default;
    bool initialize(ID3D11Device* device,
                    ID3D11DeviceContext* deviceContext, 
                    IDXGISwapChain* swapChain, 
                    ID3D11RenderTargetView* renderTargetView);
    void renderFrame(const DirectX::XMMATRIX& viewProjectionMatrix);

    void setObjects(std::vector<IRenderable*> &renderables);

private:

    std::vector<IRenderable*> *renderableObjects;

    std::wstring deviceDescription;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;

    ID3D11RasterizerState* rasterizerState = nullptr;
    ID3D11RasterizerState* rasterizerStateCullFront = nullptr;
};