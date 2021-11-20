#include "Light.h"
#include <DirectXMath.h>

using namespace DirectX;

Light::Light() {
    setPosition(0.0f, 0.0f, 0.0f);
    setRotation(0.0f, 0.0f, 0.0f);
    updateMatrix();
}

void Light::updateMatrix() {}

void DynamicLight::updateMatrix() {
    worldMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * XMMatrixTranslation(pos.x, pos.y, pos.z);
    updateDirectionVectors();
}
