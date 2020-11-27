#pragma once
#include "GameObject.h"

namespace NGraphics {
    class GameObject3D : public GameObject {
    public:

        void setLookAtPos(XMFLOAT3 lookAtPos);
        const XMVECTOR& getForwardVector(bool omitY = false);
        const XMVECTOR& getBackwardVector(bool omitY = false);
        const XMVECTOR& getLeftVector(bool omitY = false);
        const XMVECTOR& getRightVector(bool omitY = false);

    protected:
        void updateDirectionVectors();
        const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
        const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
        const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

        XMVECTOR vecForward;
        XMVECTOR vecBackward;
        XMVECTOR vecLeft;
        XMVECTOR vecRight;

        XMVECTOR vecForward_noY;
        XMVECTOR vecBackward_noY;
        XMVECTOR vecLeft_noY;
        XMVECTOR vecRight_noY;
    };
}