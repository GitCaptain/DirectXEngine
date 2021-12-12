#pragma once

#include <string>
#include <vector>
#include "Mesh.h"

namespace NModel {

    using namespace DirectX;

    class Model final {
    
    public:
        bool initialize(
            const std::string& filePath,
            ID3D11Device *device
        );
        const std::vector<Mesh>& getMeshes() const;

    private:
        bool loadModel(const std::string& filePath);
        void processNode(aiNode* node, const aiScene *scene, const XMMATRIX &parentTransformMatrix);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene, const XMMATRIX &transformMatrix);
        TextureStorageType determineTextureStorageType(
            const aiScene *pScene,
            aiMaterial *pMaterial,
            size_t index,
            aiTextureType textureType
        );
        std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
        int getTextureIndex(aiString *pStr);
        
        std::vector<Mesh> meshes;
        ID3D11Device* device = nullptr;
        std::string directory = "";
     };
}
