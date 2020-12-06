#pragma once

#include "interfaces/IRenderable.h"

namespace NGraphics {

    class RenderableObject3D : public IRenderable {
    public:
        RenderableObject3D() = default;
        virtual ~RenderableObject3D() = default;
        virtual void draw(const DirectX::XMMATRIX&) override = 0;
        virtual void prepare(ID3D11DeviceContext*) override = 0;
    };
}