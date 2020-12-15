#pragma once

#include "interfaces/IRenderable.h"

namespace NGraphics {

    class RenderableObject2D: public IRenderable {
    public: 
        RenderableObject2D() = default;
        virtual ~RenderableObject2D() = default;
        virtual void draw(const DirectX::XMMATRIX&) override = 0;
        virtual void prepare(ID3D11DeviceContext*) override = 0;
    };

}