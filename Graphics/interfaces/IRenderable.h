#pragma once

#include <DirectXMath.h>
#include <d3d11.h>

class IRenderable {
protected:
    IRenderable() = default;
    virtual ~IRenderable() = default;
public:
    virtual void draw(const DirectX::XMMATRIX&) = 0;
    virtual void prepare(ID3D11DeviceContext*) = 0;
};