#include "RenderableGameObject.h"

using namespace NGraphics;

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
    updateDirectionVectors();
}
