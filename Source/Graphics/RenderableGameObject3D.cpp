#include "RenderableGameObject3D.h"

using namespace NGraphics;

bool RenderableGameObject3D::initialize(const std::string& filePath,
    ID3D11Device* device,
    ID3D11DeviceContext* deviceContext,
    ConstantBuffer<CB_VS_m_world_viewprojeciton>& cb_vs_vertexshader) {

    if (!model.initialize(filePath, device, deviceContext, cb_vs_vertexshader)) {
        return false;
    }

    setPosition(0.0f, 0.0f, 0.0f);
    setRotation(0.0f, 0.0f, 0.0f);
    updateMatrix();

    return true;
}

void RenderableGameObject3D::draw(const XMMATRIX& viewProjectionMatrix) {
    model.draw(worldMatrix, viewProjectionMatrix);
}

void NGraphics::RenderableGameObject3D::prepare(ID3D11DeviceContext* deviceContext) {
    
}

void RenderableGameObject3D::updateMatrix() {
    worldMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * XMMatrixTranslation(pos.x, pos.y, pos.z);
    updateDirectionVectors();
}
