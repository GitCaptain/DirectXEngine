#include "Sprite.h"

using namespace NGraphics;

bool Sprite::initialize(ID3D11Device* device, 
                        ID3D11DeviceContext* deviceContext, 
                        float width, 
                        float height, 
                        std::string spritePath, 
                        ConstantBuffer<CB_VS_vertexshader_2d>& cb_vs_vertexshader_2d) {
    this->deviceContext = deviceContext;
    if (deviceContext == nullptr) {
        return false;
    }

    this->cb_vs_vertexshader_2d = &cb_vs_vertexshader_2d;

    // we don't care about texture type in this case
    texture = std::make_unique<Texture>(device, spritePath, aiTextureType::aiTextureType_NONE); 
    std::vector<Vertex2D> vertexData = {
        Vertex2D(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f), // top left
        Vertex2D(0.5f, -0.5f, 0.0f, 1.0f, 0.0f), // top right
        Vertex2D(-0.5f,  0.5f, 0.0f, 0.0f, 1.0f), // bottom left
        Vertex2D(0.5f,  0.5f, 0.0f, 1.0f, 1.0f), // bottom right
    };

    std::vector<DWORD> indexData = {
        0, 1, 2,
        2, 1, 3,
    };

    HRESULT hr = vertices.initialize(device, vertexData.data(), vertexData.size());
    COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for sprite");

    hr = indices.initialize(device, indexData.data(), indexData.size());
    COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for sprite");

    setPosition(0.0f, 0.0f, 0.0f);
    setRotation(0.0f, 0.0f, 0.0f);

    setScale(width, height);

    return true;
}

void Sprite::draw(const XMMATRIX &orthoMatrix) {
    XMMATRIX worldViewProjectionMatrix = worldMatrix * orthoMatrix;
    deviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexshader_2d->GetAddressOf());

    cb_vs_vertexshader_2d->data.worldViewProjectionMatrix = worldViewProjectionMatrix;
    cb_vs_vertexshader_2d->applyChanges();

    deviceContext->PSSetShaderResources(0, 1, texture->getTextureResourceViewAddress());

    const UINT offsets = 0;

    deviceContext->IASetVertexBuffers(0, 1, vertices.GetAddressOf(), vertices.getStridePtr(), &offsets);
    deviceContext->IASetIndexBuffer(indices.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
    deviceContext->DrawIndexed(indices.getIndexCount(), 0, 0);
}

float Sprite::getWidth() {
    return scale.x;
}

float Sprite::getHeight() {
    return scale.y;
}

void Sprite::updateMatrix() {
    worldMatrix = XMMatrixScaling(scale.x, scale.y, 1.0f) * 
                  XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * 
                  XMMatrixTranslation(pos.x + scale.x / 2.0f, pos.y + scale.y / 2.0f, pos.z);
}
