#pragma once

#include "GameObject2D.h"

namespace NGameObject {
    
    using namespace DirectX;

    class Camera2D : public GameObject2D {
    
    public:
        Camera2D();
        void setProjectionValues(float width, float height, float nearZ, float farZ);

        const XMMATRIX& getOrthoMatrix() const;
        const XMMATRIX& getWorldMatrix() const;
    
    private:
        void updateMatrix() override;
        XMMATRIX orthoMatrix;
        XMMATRIX worldMatrix;

    };

}