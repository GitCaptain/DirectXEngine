#include "Mesh.h"

Mesh::Mesh(ID3D11Device* device,
           ID3D11DeviceContext* deviceContext,
           std::vector<Vertex3D>& vertices,
           std::vector<DWORD> &indices,
           std::vector<Texture>& textures,
           const DirectX::XMMATRIX& transformMatrix) {

    this->deviceContext = deviceContext;
    this->textures = textures;
    this->transformMatrix = transformMatrix;

    HRESULT hr = vertexBuffer.initialize(device, vertices.data(), vertices.size());
    COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh");

    hr = indexBuffer.initialize(device, indices.data(), indices.size());
    COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh");
}

void Mesh::draw() {
    UINT offset = 0;

    for (auto &texture : textures) {
        if (texture.getType() == aiTextureType::aiTextureType_DIFFUSE) {
            deviceContext->PSSetShaderResources(0, 1, texture.getTextureResourceViewAddress());
            break;
        }
    }

    deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.getStridePtr(), &offset);
    deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
    deviceContext->DrawIndexed(indexBuffer.getIndexCount(), 0, 0);
}

const DirectX::XMMATRIX& Mesh::getTransformMatrix() {
    return transformMatrix;
}
