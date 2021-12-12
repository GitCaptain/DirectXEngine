#include "Mesh.h"

Mesh::Mesh(
    ID3D11Device* device,
    std::vector<Vertex3D>& vertices,
    std::vector<DWORD> &indices,
    std::vector<Texture>& textures,
    const DirectX::XMMATRIX& transformMatrix,
    float specularPower,
    float specularStrength
): textures(textures), transformMatrix(transformMatrix), specPower(specularPower), specStrength(specularStrength) {

    HRESULT hr = vertexBuffer.initialize(device, vertices.data(), vertices.size());
    COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh");

    hr = indexBuffer.initialize(device, indices.data(), indices.size());
    COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh");
}

const DirectX::XMMATRIX& Mesh::getTransformMatrix() const {
    return transformMatrix;
}

const VertexBuffer<Vertex3D>& Mesh::getVertexBuffer() const {
    return vertexBuffer;
}

const IndexBuffer& Mesh::getIndexBuffer() const {
    return indexBuffer;
}

const Texture* Mesh::getTexture(aiTextureType texType) const {
    for (const Texture &texture : textures) {
        if (texture.getType() == texType) {
            return &texture;
        }
    }
    return nullptr;
}

const std::pair<float, float> Mesh::getSpecularParams() const {
    return {specPower, specStrength};
}
