#pragma once
#include "Model.h"

namespace GameObjectNamespace {

    using namespace ModelNamespace;

    class GameObject {

    public:
        bool initialize(const std::string& filePath,
            ID3D11Device* device,
            ID3D11DeviceContext* deviceContext,
            ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
        void draw(const XMMATRIX& viewProjectionMatrix);

        const XMVECTOR& getPositionVector() const;
        const XMFLOAT3& getPositionFloat3() const;
        const XMVECTOR& getRotationVector() const;
        const XMFLOAT3& getRotationFloat3() const;

        const XMVECTOR& getForwardVector();
        const XMVECTOR& getBackwardVector();
        const XMVECTOR& getLeftVector();
        const XMVECTOR& getRightVector();

        void setPosition(const XMVECTOR& pos);
        void setPosition(const XMFLOAT3& pos);
        void setPosition(float x, float y, float z);
        void adjustPosition(const XMVECTOR& pos);
        void adjustPosition(const XMFLOAT3& pos);
        void adjustPosition(float x, float y, float z);
        void setRotation(const XMVECTOR& rot);
        void setRotation(const XMFLOAT3& rot);
        void setRotation(float x, float y, float z);
        void adjustRotation(const XMVECTOR& rot);
        void adjustRotation(const XMFLOAT3& rot);
        void adjustRotation(float x, float y, float z);
        void setLookAtPos(XMFLOAT3 lookAtPos);

    private:
        void updateWorldMatrix();
        
        Model model;
        XMMATRIX worldMatrix = XMMatrixIdentity();

        XMVECTOR posVector;
        XMVECTOR rotVector;
        XMFLOAT3 pos;
        XMFLOAT3 rot;

        const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
        const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
        const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

        XMVECTOR vecForward;
        XMVECTOR vecBackward;
        XMVECTOR vecLeft;
        XMVECTOR vecRight;
    };
}