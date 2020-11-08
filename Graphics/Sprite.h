#pragma once

#include "GameObject2D.h" 

namespace GameObjectNamespace {

    class Sprite : public GameObject2D {
    public:
        bool initialize(ID3D11Device *device, 
                        ID3D11DeviceContext* deviceContext, 
                        float width, 
                        float height, 
                        std::string spritePath, 
                        ConstantBuffer<CB_VS_vertexshader_2d> &cb_vs_vertexshader_2d);
        void draw(const XMMATRIX &orthoMatrix); // 2D camera orthogonal matrix
        float getHeight();
        float getWidth();
    private:
        void updateMatrix() override;

        ConstantBuffer<CB_VS_vertexshader_2d>* cb_vs_vertexshader_2d = nullptr;
        XMMATRIX worldMatrix = XMMatrixIdentity();

        std::unique_ptr<Texture> texture;
        ID3D11DeviceContext* deviceContext = nullptr;

        IndexBuffer indices;
        VertexBuffer<Vertex2D> vertices;
    };

}