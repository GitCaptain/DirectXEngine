#pragma once
#include "GameObject.h"

namespace NGameObject {
    class GameObject3D : public GameObject {
    public:

        void setLookAtPos(XMFLOAT3 lookAtPos);
        const XMVECTOR& getForwardVector(bool omitY = false);
        const XMVECTOR& getBackwardVector(bool omitY = false);
        const XMVECTOR& getLeftVector(bool omitY = false);
        const XMVECTOR& getRightVector(bool omitY = false);

    protected:
        void updateDirectionVectors();
        const static XMVECTOR DEFAULT_FORWARD_VECTOR;
        const static XMVECTOR DEFAULT_BACKWARD_VECTOR;
        const static XMVECTOR DEFAULT_UP_VECTOR;
        const static XMVECTOR DEFAULT_LEFT_VECTOR;
        const static XMVECTOR DEFAULT_RIGHT_VECTOR;

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