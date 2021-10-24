
#include "DXUtils.h"

using namespace DirectX;

XMFLOAT3 operator*(const XMFLOAT3& v, const float s) {
	return XMFLOAT3(v.x * s, v.y *s, v.z *s);
}

DirectX::XMFLOAT3 operator/(const DirectX::XMFLOAT3& v1, const float s) {
	return DirectX::XMFLOAT3(v1.x / s, v1.y / s, v1.z / s);
}

DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) {
	return DirectX::XMFLOAT3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) {
	return DirectX::XMFLOAT3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
