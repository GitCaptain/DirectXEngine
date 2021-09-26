#include "GameObject3D.h"

using namespace GameObjectNamespace;

void GameObject3D::setLookAtPos(XMFLOAT3 lookAtPos) {
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

const XMVECTOR& GameObject3D::getForwardVector(bool omitY) {
    if (omitY) {
        return vecForward_noY;
    }
    return vecForward;
}

const XMVECTOR& GameObject3D::getBackwardVector(bool omitY) {
    if (omitY) {
        return vecBackward_noY;
    }
    return vecBackward;
}

const XMVECTOR& GameObject3D::getLeftVector(bool omitY) {
    if (omitY) {
        return vecLeft_noY;
    }
    return vecLeft;
}

const XMVECTOR& GameObject3D::getRightVector(bool omitY) {
    if (omitY) {
        return vecRight_noY;
    }
    return vecRight;
}

void GameObject3D::updateDirectionVectors() {
    XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, 0.0f);
    vecForward = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
    vecBackward = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
    vecLeft = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrix);
    vecRight = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrix);

    XMMATRIX vecRotationMatrixNoY = XMMatrixRotationRollPitchYaw(rot.x, rot.y, 0.0f);
    vecForward_noY = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vecRotationMatrixNoY);
    vecBackward_noY = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrixNoY);
    vecLeft_noY = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrixNoY);
    vecRight_noY = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrixNoY);
}
