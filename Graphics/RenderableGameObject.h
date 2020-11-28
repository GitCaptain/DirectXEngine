#pragma once

#include "GameObject3D.h"
#include "interfaces/IRenderable.h"

namespace NGraphics { 

    class RenderableGameObject: public GameObject3D, public IRenderable {

    public:
        bool initialize(const std::string& filePath,
            ID3D11Device* device,
            ID3D11DeviceContext* deviceContext,
            ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
        void draw(const XMMATRIX& viewProjectionMatrix) override;
    
    protected:
        void updateMatrix() override;

        Model model;
        XMMATRIX worldMatrix = XMMatrixIdentity();
    };
}