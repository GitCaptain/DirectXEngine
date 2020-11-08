#include "RenderableGameObject.h"

using namespace GameObjectNamespace;

bool RenderableGameObject::initialize(const std::string& filePath,
    ID3D11Device* device,
    ID3D11DeviceContext* deviceContext,
    ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader) {

    if (!model.initialize(filePath, device, deviceContext, cb_vs_vertexshader)) {
        return false;
    }

    setPosition(0.0f, 0.0f, 0.0f);
    setRotation(0.0f, 0.0f, 0.0f);
    updateMatrix();

    return true;
}

void RenderableGameObject::draw(const XMMATRIX& viewProjectionMatrix) {
    model.draw(worldMatrix, viewProjectionMatrix);
}

void RenderableGameObject::updateMatrix() {
    worldMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * XMMatrixTranslation(pos.x, pos.y, pos.z);
    XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, rot.y, 0.0f);
    vecForward = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
    vecBackward = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
    vecLeft = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrix);
    vecRight = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
}
