#pragma once
#include <DirectXMath.h>

struct Vertex3D {
    Vertex3D() = default;
    Vertex3D(float x, float y, float z, float u, float v, float nx, float ny, float nz):
           pos(x, y, z), texCoord(u, v), normal(nx, ny, nz)  {}
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 texCoord;
    DirectX::XMFLOAT3 normal;
};

struct VertexPosition3D {
    VertexPosition3D() = default;
    VertexPosition3D(float x, float y): pos(x, y) {}
    DirectX::XMFLOAT2 pos;
};

struct Vertex2D {
    Vertex2D() = default;
    Vertex2D(float x, float y, float z, float u, float v):
        pos(x, y, z), texCoord(u, v) {}
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 texCoord;
};