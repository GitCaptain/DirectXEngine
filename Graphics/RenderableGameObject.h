#pragma once

#include "GameObject3D.h"

namespace GameObjectNamespace {

    class RenderableGameObject: public GameObject3D {

    public:
        bool initialize(const std::string& filePath,
            ID3D11Device* device,
            ID3D11DeviceContext* deviceContext,
            ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
        void draw(const XMMATRIX& viewProjectionMatrix);
    
    protected:
        void updateMatrix() override;

        Model model;
        XMMATRIX worldMatrix = XMMatrixIdentity();
    };
}