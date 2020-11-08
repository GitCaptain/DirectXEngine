#include "GameObject.h"

using namespace GameObjectNamespace;

const XMVECTOR& GameObject::getPositionVector() const {
    return posVector;
}

const XMFLOAT3& GameObject::getPositionFloat3() const {
    return pos;
}

const XMVECTOR& GameObject::getRotationVector() const {
    return rotVector;
}

const XMFLOAT3& GameObject::getRotationFloat3() const {
    return rot;
}

const XMVECTOR& GameObject::getForwardVector() {
    return vecForward;
}

const XMVECTOR& GameObject::getBackwardVector() {
    return vecBackward;
}

const XMVECTOR& GameObject::getLeftVector() {
    return vecLeft;
}

const XMVECTOR& GameObject::getRightVector() {
    return vecRight;
}

void GameObject::setPosition(const XMVECTOR& pos) {
    XMStoreFloat3(&this->pos, pos);
    posVector = pos;
    updateMatrix();
}

void GameObject::setPosition(const XMFLOAT3& pos) {
    this->pos = pos;
    posVector = XMLoadFloat3(&this->pos);
    updateMatrix();

}

void GameObject::setPosition(float x, float y, float z) {
    pos = XMFLOAT3(x, y, z);
    posVector = XMLoadFloat3(&pos);
    updateMatrix();
}

void GameObject::adjustPosition(const XMVECTOR& pos) {
    posVector += pos;
    XMStoreFloat3(&this->pos, posVector);
    updateMatrix();
}

void GameObject::adjustPosition(const XMFLOAT3& pos) {
    this->pos.x += pos.x;
    this->pos.y += pos.y;
    this->pos.z += pos.z;
    posVector = XMLoadFloat3(&this->pos);
    this->updateMatrix();
}

void GameObject::adjustPosition(float x, float y, float z) {
    pos.x += x;
    pos.y += y;
    pos.z += z;
    posVector = XMLoadFloat3(&pos);
    updateMatrix();
}

void GameObject::setRotation(const XMVECTOR& rot) {
    rotVector = rot;
    XMStoreFloat3(&this->rot, rot);
    updateMatrix();
}

void GameObject::setRotation(const XMFLOAT3& rot) {
    this->rot = rot;
    rotVector = XMLoadFloat3(&this->rot);
    updateMatrix();
}

void GameObject::setRotation(float x, float y, float z) {
    rot = XMFLOAT3(x, y, z);
    rotVector = XMLoadFloat3(&rot);
    updateMatrix();
}

void GameObject::adjustRotation(const XMVECTOR& rot) {
    rotVector += rot;
    XMStoreFloat3(&this->rot, rotVector);
    updateMatrix();
}

void GameObject::adjustRotation(const XMFLOAT3& rot) {
    this->rot.x += rot.x;
    this->rot.y += rot.y;
    this->rot.z += rot.z;
    rotVector = XMLoadFloat3(&this->rot);
    updateMatrix();
}

void GameObject::adjustRotation(float x, float y, float z) {
    rot.x += x;
    rot.y += y;
    rot.z += z;
    rotVector = XMLoadFloat3(&this->rot);
    updateMatrix();
}

void GameObject::setLookAtPos(XMFLOAT3 lookAtPos) {
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

void GameObject::updateMatrix() {
    assert("updateMatrix must be overriden" && 0);
}
