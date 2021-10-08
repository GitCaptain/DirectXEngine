#include "Camera3D.h"

using namespace NGameObject;

Camera3D::Camera3D() {
    pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    posVector = XMLoadFloat3(&pos);
    rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
    rotVector = XMLoadFloat3(&rot);
    updateMatrix();
}

void Camera3D::setProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ) {
    float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
    projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

const XMMATRIX& Camera3D::getViewMatrix() const {
    return viewMatrix;
}

const XMMATRIX& Camera3D::getProjectionMatrix() const {
    return projectionMatrix;
}

// updates view matrix and movement vectors
void Camera3D::updateMatrix() {

    //calculate camera rotation matrix
    XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
    // calculate unit vectot of camera target based off camera forward value transformed by camera rotation
    XMVECTOR camTarget = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, camRotationMatrix);
    // adjust camera target to be offset by camera's current position
    camTarget += posVector;
    // calculate up direction based on current rotation
    XMVECTOR upDir = XMVector3TransformCoord(DEFAULT_UP_VECTOR, camRotationMatrix);
    // rebuild view matrix
    viewMatrix = XMMatrixLookAtLH(posVector, camTarget, upDir);

    updateDirectionVectors();
}
