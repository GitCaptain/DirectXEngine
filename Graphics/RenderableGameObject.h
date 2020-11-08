#pragma once

#include "GameObject.h"

namespace GameObjectNamespace {

    class RenderableGameObject: public GameObject {
    public:
        bool initialize(const std::string& filePath,
            ID3D11Device* device,
            ID3D11DeviceContext* deviceContext,
            ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
        void draw(const XMMATRIX& viewProjectionMatrix);
    private:
        void updateMatrix() override;

        Model model;
        XMMATRIX worldMatrix = XMMatrixIdentity();
    };
}