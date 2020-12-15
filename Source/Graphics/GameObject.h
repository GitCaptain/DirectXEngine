#pragma once
#include "Model.h"

namespace NGraphics {

    using namespace DirectX;

    class GameObject {

    public:

        enum class game_object_type {
            TYPE_BASE,
            TYPE_2D,
            TYPE_3D,
        };

        GameObject() = default;
        virtual ~GameObject() = default;

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

        virtual game_object_type getGameObjectType();

    protected:

        virtual void updateMatrix() = 0;

        XMVECTOR posVector;
        XMVECTOR rotVector;
        XMFLOAT3 pos;
        XMFLOAT3 rot;

        XMFLOAT3 scale;
    };
}
