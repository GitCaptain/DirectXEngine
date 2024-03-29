#include <stdexcept>
#include <format>
#include <cmath>
#include "../ErrorLogger.h"
#include "Model.h"
using namespace NModel;

bool FloatEqual(const float a, const float b, const float epsilon = 5e-7) {
#ifndef NDEBUG
    const auto diff = fabs(a - b);
    if (diff < epsilon) {
        return true;
    }
    ErrorLogger::log(std::format("The difference is: {}", diff));
    return false;
#else
    return fabs(a - b) < epsilon;
#endif
}

bool Model::initialize(
    const std::string& filePath,
    ID3D11Device* device
) {
    this->device = device;

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

const std::vector<Mesh>& Model::getMeshes() const {
    return meshes;
}

bool Model::loadModel(const std::string& filePath) {

    directory = StringHelper::getDirectoryFromPath(filePath);

    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(filePath,
        aiProcess_ConvertToLeftHanded |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenNormals
    );

    if (pScene == nullptr) {
        ErrorLogger::log("Failed to load model at path: " + filePath);
        return false;
    }

    processNode(pScene->mRootNode, pScene, XMMatrixIdentity());

    return true;
}

void Model::processNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix) {
    
    XMMATRIX nodeTransformMatrix = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1)) * parentTransformMatrix;

    for (size_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, nodeTransformMatrix));
    }

    for (size_t i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, nodeTransformMatrix);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix) {
    std::vector<Vertex3D> vertices;
    std::vector<DWORD> indices;
    for (size_t i = 0; i < mesh->mNumVertices; i++) {
        Vertex3D vertex;
        vertex.pos.x = mesh->mVertices[i].x;
        vertex.pos.y = mesh->mVertices[i].y;
        vertex.pos.z = mesh->mVertices[i].z;

        const auto normalVector = mesh->mNormals[i].Normalize();
        //following assertion fails without prior call to Normalize,
        //so we can't trust to model's vector and should normalize ourselves
        assert(FloatEqual(normalVector.SquareLength(), 1) && "Normal length doesn't equal to 1!");

        vertex.normal.x = normalVector.x;
        vertex.normal.y = normalVector.y;
        vertex.normal.z = normalVector.z;

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
    std::vector<Texture> specularTex = LoadMaterialTextures(material, aiTextureType_SPECULAR, scene);

    float specPower, specStrength;
    aiReturn load;
    load = material->Get(AI_MATKEY_SHININESS, specPower);
    if (load != aiReturn_SUCCESS) {
        specPower = 1.0f;
    }
    load = material->Get(AI_MATKEY_SHININESS_STRENGTH, specStrength);
    if(load != aiReturn_SUCCESS) {
        specStrength = 1.0f;
    }
    textures.insert(textures.end(), specularTex.begin(), specularTex.end());
    return Mesh(device, vertices, indices, textures, transformMatrix, specPower, specStrength);
}

TextureStorageType Model::determineTextureStorageType(
    const aiScene* pScene,
    aiMaterial* pMaterial,
    size_t index,
    aiTextureType textureType
) {
    
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
            assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSED TEXTURES" && false);
            return TextureStorageType::EmbeddedIndexNonCompressed;
        }
    }

    // check if texture is an embedded texture, but not indexed
    // (path will be texture's name instead of #)
    auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str());
    if (pTex != nullptr) {
        if (pTex->mHeight == 0) {
            return TextureStorageType::EmbeddedCompressed;
        }
        else {
            assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSED TEXTURES" && false);
            return TextureStorageType::EmbeddedNonCompressed;
        }
    }

    // Lastly check if texture is a filepath
    // by checking for a period before extension name
    if (texturePath.find('.') != std::string::npos) {
        return TextureStorageType::Disk;
    }

    // we shouldn't get here
    return TextureStorageType::None;
} 

std::vector<Texture> Model::LoadMaterialTextures(
    aiMaterial* pMaterial,
    aiTextureType textureType,
    const aiScene* pScene
) {
    std::vector<Texture> materialTextures;
    TextureStorageType storageType = TextureStorageType::Invalid;
    unsigned int textureCount = pMaterial->GetTextureCount(textureType);

    if (textureCount == 0) { // no textures =>
        storageType = TextureStorageType::None;
        aiColor3D aiColor(0.0f, 0.0f, 0.0f);
        auto emplaceTexture =
            [&materialTextures=materialTextures, &device=device, &textureType=textureType, &aiColor=aiColor]
            (const aiReturn& colorLoadStatus, const Colors::Color& ExceptionColor)
        {
            if (colorLoadStatus != AI_SUCCESS) {
                materialTextures.emplace_back(device, ExceptionColor, textureType);
            }
            else {
                materialTextures.emplace_back(device, Colors::Color(aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), textureType);
            }
        };
        switch (textureType) {
            case aiTextureType_DIFFUSE: {
                const aiReturn colorLoadStatus = pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
                emplaceTexture(colorLoadStatus, Colors::UnloadedTextureColor);
                return materialTextures;
            }
            case aiTextureType_SPECULAR: {
                const aiReturn colorLoadStatus = pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, aiColor);
                emplaceTexture(colorLoadStatus, Colors::NoSpecularTextureColor);
                return materialTextures;
            }
            default:
                throw std::invalid_argument(std::format("Unexpected AiTextureType: {}", static_cast<int>(textureType)));
        }
    }
    for (size_t i = 0; i < textureCount; i++) {
        aiString path;
        pMaterial->GetTexture(textureType, i, &path);
        TextureStorageType storageType = determineTextureStorageType(pScene, pMaterial, i, textureType);
        switch (storageType) {
            case TextureStorageType::EmbeddedIndexCompressed: {
                int index = getTextureIndex(&path);
                materialTextures.emplace_back(
                    device,
                    reinterpret_cast<const uint8_t*>(pScene->mTextures[index]->pcData),
                    pScene->mTextures[index]->mWidth, // actual size in bytes
                    textureType
                );
                break;
            }
            case TextureStorageType::EmbeddedCompressed: {
                const aiTexture* pTexture = pScene->GetEmbeddedTexture(path.C_Str());
                materialTextures.emplace_back(
                    device,
                    reinterpret_cast<const uint8_t*>(pTexture->pcData),
                    pTexture->mWidth, // actual size in bytes
                    textureType
                );
                break;
            }
            case TextureStorageType::Disk: {
                std::string fileName = directory + '\\' + path.C_Str();
                materialTextures.emplace_back(device, fileName, textureType);
                break;
            }
            default:
                throw std::invalid_argument(std::format("Unexpected AiTextureType: {}", static_cast<int>(textureType)));
        }
    }

    if (materialTextures.empty()) {
        materialTextures.emplace_back(device, Colors::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE);
    }

    return materialTextures;
}

int Model::getTextureIndex(aiString* pStr) {
    assert(pStr->length >= 2);
    return atoi(&pStr->C_Str()[1]);
}
