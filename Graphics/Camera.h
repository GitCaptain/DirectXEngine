#pragma once

#include <DirectXMath.h>
namespace Camera {

    using namespace DirectX;

    class Camera {
 
    public:
        Camera();
        void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
        
        const XMMATRIX& getViewMatrix() const;
        const XMMATRIX& getProjectionMatrix() const;

        const XMVECTOR& getPositionVector() const; 
        const XMFLOAT3& getPositionFloat3() const; 
        const XMVECTOR& getRotationVector() const; 
        const XMFLOAT3& getRotationFloat3() const; 

        const XMVECTOR& getForwardVector();
        const XMVECTOR& getBackwardVector();
        const XMVECTOR& getLeftVector();
        const XMVECTOR& getRightVector();

        void setPosition(const XMVECTOR& pos);
        void setPosition(float x, float y, float z);
        void adjustPosition(const XMVECTOR& pos);
        void adjustPosition(float x, float y, float z);        
        void setRotation(const XMVECTOR& rot);
        void setRotation(float x, float y, float z);
        void adjustRotation(const XMVECTOR& rot);
        void adjustRotation(float x, float y, float z);
        void setLookAtPos(XMFLOAT3 lookAtPos);

    private:
        void updateViewMatrix();
        XMVECTOR posVector;
        XMVECTOR rotVector;
        XMFLOAT3 pos;
        XMFLOAT3 rot;
        XMMATRIX viewMatrix;
        XMMATRIX projectionMatrix;

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
