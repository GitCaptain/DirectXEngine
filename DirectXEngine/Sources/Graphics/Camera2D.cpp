#include "Camera2D.h"

using namespace NGameObject;

Camera2D::Camera2D() {
    pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    posVector = XMLoadFloat3(&pos);
    rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
    rotVector = XMLoadFloat3(&rot);
    updateMatrix();
}

void Camera2D::setProjectionValues(float width, float height, float nearZ, float farZ) {
    orthoMatrix = XMMatrixOrthographicOffCenterLH(0.0f, width, height, 0.0f, nearZ, farZ);
}

const XMMATRIX& Camera2D::getOrthoMatrix() const {
    return orthoMatrix;
}

const XMMATRIX& Camera2D::getWorldMatrix() const {
    return worldMatrix;
}

void Camera2D::updateMatrix() {
    XMMATRIX translationOffsetMatrix = XMMatrixTranslation(-pos.x, -pos.y, 0.0f); //z component irrelevant for 2d camera
    XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
    worldMatrix = camRotationMatrix * translationOffsetMatrix;
}
