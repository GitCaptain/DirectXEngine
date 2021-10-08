#pragma once
#include "Model.h"

namespace NGameObject {

    using namespace NModel;

    class GameObject {

    public:
        const XMVECTOR& getPositionVector() const;
        const XMFLOAT3& getPositionFloat3() const;
        const XMVECTOR& getRotationVector() const;
        const XMFLOAT3& getRotationFloat3() const;

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

        void setScale(float xScale, float yScale, float zScale = 1.0f);

        virtual ~GameObject() = default;

    protected:

        virtual void updateMatrix();

        XMVECTOR posVector;
        XMVECTOR rotVector;
        XMFLOAT3 pos;
        XMFLOAT3 rot;

        XMFLOAT3 scale;
    };
}
