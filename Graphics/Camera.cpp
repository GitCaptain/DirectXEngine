#include "Camera.h"

using namespace DirectX;

Camera::Camera::Camera() {
    pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    posVector = XMLoadFloat3(&pos);
    rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
    rotVector = XMLoadFloat3(&rot);
    updateViewMatrix();
}

void Camera::Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ) {
    float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
    projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

const XMMATRIX& Camera::Camera::getViewMatrix() const {
    return viewMatrix;
}

const XMMATRIX& Camera::Camera::getProjectionMatrix() const {
    return projectionMatrix;
}

const XMVECTOR& Camera::Camera::getPositionVector() const {
    return posVector;
}

const XMFLOAT3& Camera::Camera::getPositionFloat3() const {
    return pos;
}

const XMVECTOR& Camera::Camera::getRotationVector() const {
    return rotVector;
}

const XMFLOAT3& Camera::Camera::getRotationFloat3() const {
    return rot;
}

void Camera::Camera::setPosition(const XMVECTOR& pos) {
    XMStoreFloat3(&this->pos, pos);
    posVector = pos;
    updateViewMatrix();
}

void Camera::Camera::setPosition(float x, float y, float z) {
    pos = XMFLOAT3(x, y, z);
    posVector = XMLoadFloat3(&pos);
    updateViewMatrix();
}

void Camera::Camera::adjustPosition(const XMVECTOR& pos) {  
    posVector += pos;
    XMStoreFloat3(&this->pos, posVector);
    updateViewMatrix();
}

void Camera::Camera::adjustPosition(float x, float y, float z) {
    pos.x += x;
    pos.y += y;
    pos.z += z;
    posVector = XMLoadFloat3(&pos);
    updateViewMatrix();
}

void Camera::Camera::setRotation(const XMVECTOR& rot) {
    rotVector = rot;
    XMStoreFloat3(&this->rot, rot);
    updateViewMatrix();
}

void Camera::Camera::setRotation(float x, float y, float z) {
    rot = XMFLOAT3(x, y, z);
    rotVector = XMLoadFloat3(&rot);
    updateViewMatrix();
}

void Camera::Camera::adjustRotation(const XMVECTOR& rot) {
    rotVector += rot;
    XMStoreFloat3(&this->rot, rotVector);
    updateViewMatrix();
}

void Camera::Camera::adjustRotation(float x, float y, float z) {
    rot.x += x;
    rot.y += y;
    rot.z += z;
    rotVector = XMLoadFloat3(&this->rot);
    updateViewMatrix();
}

void Camera::Camera::setLookAtPos(XMFLOAT3 lookAtPos) {
    // veirfy that look at pos is not the same as camera pos. 
    // they cannot be the same as that wouldn't make sense and would result in undefined behavior.
    if (lookAtPos.x == pos.x && lookAtPos.y == pos.y && lookAtPos.z == pos.z) {
        return;
    }

    lookAtPos.x = pos.x - lookAtPos.x;
    lookAtPos.y = pos.y - lookAtPos.y;
    lookAtPos.z = pos.z - lookAtPos.z;

    float pitch = 0.0f;
    if (lookAtPos.y != 0.0f) {
        const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
        pitch = atan(lookAtPos.y / distance);
    }

    float yaw = 0.0f;
    if (lookAtPos.x != 0.0f) {
        yaw = atan(lookAtPos.x / lookAtPos.z);
    }
    if (lookAtPos.z > 0) {
        yaw += XM_PI;
    }

    setRotation(pitch, yaw, 0.0f);

}


// updates view matrix and movement vectors
void Camera::Camera::updateViewMatrix() {

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
}
