#pragma once

#include <DirectXMath.h>

DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& v, const float s);
DirectX::XMFLOAT3 operator/(const DirectX::XMFLOAT3& v1, const float s);
DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2);
DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2);
