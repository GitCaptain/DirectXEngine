#include "Model.h"

using namespace ModelNamespace;

bool Model::initialize(const std::string& filePath, 
                            ID3D11Device* device,
                            ID3D11DeviceContext* deviceContext,
                            ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader) {
    this->device = device;
    this->deviceContext = deviceContext;
    this->cb_vs_vertexshader = &cb_vs_vertexshader;

    try {
        if (!loadModel(filePath)) {
            return false;
        }
    }
    catch (const COMException& e) {
        ErrorLogger::log(e);
        return false;
    }

    return true; 
}

void Model::draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix) {
    cb_vs_vertexshader->data.mat = worldMatrix * viewProjectionMatrix;
    cb_vs_vertexshader->data.mat = XMMatrixTranspose(cb_vs_vertexshader->data.mat);
    cb_vs_vertexshader->applyChanges();
    deviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexshader->GetAddressOf());

    for (size_t i = 0; i < meshes.size(); i++) {
        meshes[i].draw();
    }
}

bool Model::loadModel(const std::string& filePath) {

    directory = StringHelper::getDirectoryFromPath(filePath);

    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

    if (pScene == nullptr) {
        return false;
    }

    processNode(pScene->mRootNode, pScene);

    return true;
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    for (size_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (size_t i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<DWORD> indices;
    for (size_t i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.pos.x = mesh->mVertices[i].x;
        vertex.pos.y = mesh->mVertices[i].y;
        vertex.pos.z = mesh->mVertices[i].z;

        // TODO: what if mesh doesn't have a texture?
        if (mesh->mTextureCoords[0]) {
            vertex.texCoord.x = static_cast<float>(mesh->mTextureCoords[0][i].x);
            vertex.texCoord.y = static_cast<float>(mesh->mTextureCoords[0][i].y);
        }

        vertices.push_back(vertex);
    }

    for (size_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace *face = &mesh->mFaces[i];
        for (size_t j = 0; j < face->mNumIndices; j++) {
            indices.push_back(face->mIndices[j]);
        }
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> textures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, scene);

    return Mesh(device, deviceContext, vertices, indices, textures);
}

TextureStorageType ModelNamespace::Model::determineTextureStorageType(const aiScene* pScene, 
                                                                      aiMaterial* pMaterial, 
                                                                      size_t index, 
                                                                      aiTextureType textureType) {
    
    if (pMaterial->GetTextureCount(textureType) == 0) {
        // we shouldn't get here
        return TextureStorageType::None;
    }
    aiString path;
    pMaterial->GetTexture(textureType, index, &path);
    std::string texturePath = path.C_Str();

    // check if texture is an embedded indexed texture 
    // by seeing if the file path is an index #
    if (texturePath[0] == '*') {
        if (pScene->mTextures[0]->mHeight == 0) {
            return TextureStorageType::EmbeddedIndexCompressed;
        }
        else {
            // TODO: fix this strange assert usage and next usages too
            assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSED TEXTURES" && 0);
            return TextureStorageType::EmbeddedIndexNonCompressed;
        }
    }

    // check if texture is an embedded texture, but not indexed
    // (path will be texture's name instread of #)
    auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str());
    if (pTex != nullptr) {
        if (pTex->mHeight == 0) {
            return TextureStorageType::EmbeddedCompressed;
        }
        else {
            assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSED TEXTURES" && 0);
            return TextureStorageType::EmbeddedNonCompressed;
        }
    }

    // Lastly checl if texture is a filepath 
    // by checking for a period before extension name
    if (texturePath.find('.') != std::string::npos) {
        return TextureStorageType::Disk;
    }

    // we shouldn't get here
    return TextureStorageType::None;
} 

std::vector<Texture> ModelNamespace::Model::LoadMaterialTextures(aiMaterial* pMaterial, 
                                                                 aiTextureType textureType, 
                                                                 const aiScene* pScene) {
    std::vector<Texture> materialTextures;
    TextureStorageType storageType = TextureStorageType::Invalid;
    unsigned int textureCount = pMaterial->GetTextureCount(textureType);

    if (textureCount == 0) { // no textures =>
        storageType = TextureStorageType::None;
        aiColor3D aiColor(0.0f, 0.0f, 0.0f);
        switch (textureType) {
            case aiTextureType_DIFFUSE: {
                pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
                if (aiColor.IsBlack()) { // in case of black color, we use grey
                    materialTextures.emplace_back(device, Colors::UnloadedTextureColor, textureType);
                }
                else {
                    materialTextures.emplace_back(device, Colors::Color(aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), textureType);
                }
                return materialTextures;
            }
        }
    }
    else {
        for (size_t i = 0; i < textureCount; i++) {
            aiString path;
            pMaterial->GetTexture(textureType, i, &path);
            TextureStorageType storageType = determineTextureStorageType(pScene, pMaterial, i, textureType);
            switch (storageType) {
                case TextureStorageType::EmbeddedCompressed: {
                    const aiTexture* pTexture = pScene->GetEmbeddedTexture(path.C_Str());
                    materialTextures.emplace_back(device,
                                                  reinterpret_cast<const uint8_t*>(pTexture->pcData),
                                                  pTexture->mWidth, // actual size in bytes 
                                                  textureType);
                }
                case TextureStorageType::Disk: {
                    std::string fileName = directory + '\\' + path.C_Str();
                    materialTextures.emplace_back(device, fileName, textureType);
                    break;
                }
            }
        }
    }

    if (materialTextures.empty()) {
        materialTextures.emplace_back(device, Colors::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE);
    }

    return materialTextures;
}
