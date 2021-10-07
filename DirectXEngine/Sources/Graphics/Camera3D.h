#pragma once

#include <DirectXMath.h>
#include "GameObject3D.h"

namespace Camera {

    using namespace DirectX;

    class Camera3D: public NGameObject::GameObject3D {
 
    public:
        Camera3D();
        void setProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
        const XMMATRIX& getViewMatrix() const;
        const XMMATRIX& getProjectionMatrix() const;

    private:
        void updateMatrix() override;

        XMMATRIX viewMatrix;
        XMMATRIX projectionMatrix;
    };
}
