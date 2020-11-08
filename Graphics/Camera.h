#pragma once

#include <DirectXMath.h>
#include "GameObject.h"

namespace Camera {

    using namespace DirectX;

    class Camera: public GameObjectNamespace::GameObject {
 
    public:
        Camera();
        void setProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
        const XMMATRIX& getViewMatrix() const;
        const XMMATRIX& getProjectionMatrix() const;

    private:
        void updateMatrix() override;

        XMMATRIX viewMatrix;
        XMMATRIX projectionMatrix;
    };
}
