#pragma once

#include <string>
#include <vector>
#include "Mesh.h"

namespace NGraphics {

    using namespace DirectX;

    class Model {
    
    public:
        bool initialize(const std::string& filePath,
                        ID3D11Device *device, 
                        ID3D11DeviceContext *deviceContext,
                        ConstantBuffer<CB_VS_vertexshader> &cb_vs_vertexshader);
        void draw(const XMMATRIX& worldMatrix, const XMMATRIX &viewProjectionMatrix);

    private:
        bool loadModel(const std::string& filePath);
        void processNode(aiNode* node, const aiScene *scene, const XMMATRIX &parentTransformMatrix);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene, const XMMATRIX &transformMatrix);
        TextureStorageType determineTextureStorageType(const aiScene *pScene, 
                                                       aiMaterial *pMaterial, 
                                                       size_t index, 
                                                       aiTextureType textureType);
        std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
        int getTextureIndex(aiString *pStr);
        
        std::vector<Mesh> meshes;
        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* deviceContext = nullptr;
        ConstantBuffer<CB_VS_vertexshader> *cb_vs_vertexshader = nullptr;
        std::string directory = "";
     };
}
