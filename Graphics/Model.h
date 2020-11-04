#pragma once

#include <string>
#include <vector>
#include "Mesh.h"

namespace ModelNamespace {

    using namespace DirectX;

    class Model {
    
    public:
        bool initialize(const std::string& filePath,
                        ID3D11Device *device, 
                        ID3D11DeviceContext *deviceContext, 
                        ID3D11ShaderResourceView *texture, 
                        ConstantBuffer<CB_VS_vertexshader> &cb_vs_vertexshader);
        void setTexture(ID3D11ShaderResourceView *texture);
        void draw(const XMMATRIX& worldMatrix, const XMMATRIX &viewProjectionMatrix);

    private:
        bool loadModel(const std::string& filePath);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);

        std::vector<Mesh> meshes;
        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* deviceContext = nullptr;
        ConstantBuffer<CB_VS_vertexshader> *cb_vs_vertexshader = nullptr;
        ID3D11ShaderResourceView* texture = nullptr;

    };
}
