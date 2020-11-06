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

    std::vector<Texture> textures;
    textures.push_back(Texture(device, Colors::UnloadedTextureColor, aiTextureType::aiTextureType_DIFFUSE));

    return Mesh(device, deviceContext, vertices, indices, textures);
}
