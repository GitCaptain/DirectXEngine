#include "RenderableGameObject.h"

using namespace NGameObject;

bool RenderableGameObject::initialize(
    const std::string& filePath,
    ID3D11Device* device
) {

    if (!model.initialize(filePath, device)) {
        return false;
    }

    setPosition(0.0f, 0.0f, 0.0f);
    setRotation(0.0f, 0.0f, 0.0f);
    updateMatrix();

    return true;
}

const XMMATRIX& RenderableGameObject::getWorldMatrix() const {
    return worldMatrix;
}

const Model& RenderableGameObject::getModel() const {
    return model;
}

void RenderableGameObject::updateMatrix() {
    worldMatrix = XMMatrixScaling(scale.x, scale.y, scale.z) * XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * XMMatrixTranslation(pos.x, pos.y, pos.z);
    updateDirectionVectors();
}
