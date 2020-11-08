#include "Camera.h"

using namespace DirectX;

Camera::Camera::Camera() {
    pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    posVector = XMLoadFloat3(&pos);
    rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
    rotVector = XMLoadFloat3(&rot);
    updateMatrix();
}

void Camera::Camera::setProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ) {
    float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
    projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

const XMMATRIX& Camera::Camera::getViewMatrix() const {
    return viewMatrix;
}

const XMMATRIX& Camera::Camera::getProjectionMatrix() const {
    return projectionMatrix;
}

// updates view matrix and movement vectors
void Camera::Camera::updateMatrix() {

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

    XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, rot.y, 0.0f);
    vecForward = XMVector3Transform(DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
    vecBackward = XMVector3Transform(DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
    vecLeft = XMVector3Transform(DEFAULT_LEFT_VECTOR, vecRotationMatrix);
    vecRight = XMVector3Transform(DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
}
