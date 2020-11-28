#pragma once
#include <d3d11.h>

// TODO: maybe ot useful
class IRenderable {
protected:
    IRenderable() = default;
    virtual ~IRenderable() = default;
    virtual void draw(const DirectX::XMMATRIX& viewProjectionMatrix) = 0;
};