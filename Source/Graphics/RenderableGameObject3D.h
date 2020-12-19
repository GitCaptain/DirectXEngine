#pragma once

#include "GameObject3D.h"
#include "RenderableObject3D.h"

namespace NGraphics { 

    class RenderableGameObject3D: public GameObject3D, public RenderableObject3D {

    public:
        RenderableGameObject3D() = default;
        virtual ~RenderableGameObject3D() = default;
        
        bool initialize(const std::string& filePath,
            ID3D11Device* device,
            ID3D11DeviceContext* deviceContext,
            ConstantBuffer<CB_VS_m_world_viewprojeciton>& cb_vs_vertexshader);

        void draw(const XMMATRIX& viewProjectionMatrix) override;
        void prepare(ID3D11DeviceContext* deviceContext) override;

    protected:
        void updateMatrix() override;

        Model model;
        XMMATRIX worldMatrix = XMMatrixIdentity();
    };
}