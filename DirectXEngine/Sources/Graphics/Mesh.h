#pragma once

#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h" 
#include "ConstantBuffer.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "Texture.h"

class Mesh final {
public:
    Mesh(
        ID3D11Device* device,
        std::vector<Vertex3D>& vertices,
        std::vector<DWORD>& indices,
        std::vector<Texture> &textures,
        const DirectX::XMMATRIX &transformMatrix,
        float specularPower,
        float specularStrength
    );
    Mesh(const Mesh &mesh) = default;
    const DirectX::XMMATRIX& getTransformMatrix() const;
    const VertexBuffer<Vertex3D>& getVertexBuffer() const;
    const IndexBuffer& getIndexBuffer() const;
    const Texture* getTexture(aiTextureType texType = aiTextureType_DIFFUSE) const;
    const std::pair<float, float> getSpecularParams() const;
private:
    VertexBuffer<Vertex3D> vertexBuffer;
    IndexBuffer indexBuffer;
    std::vector<Texture> textures;
    DirectX::XMMATRIX transformMatrix;
    float specPower;
    float specStrength;
};
