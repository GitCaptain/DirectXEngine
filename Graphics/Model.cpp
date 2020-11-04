#include "Model.h"

using namespace Model;

bool ModelClass::initialize(const std::string& filePath, 
                            ID3D11Device* device,
                            ID3D11DeviceContext* deviceContext, 
                            ID3D11ShaderResourceView* texture, 
                            ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader) {
    this->device = device;
    this->deviceContext = deviceContext;
    this->texture = texture;
    this->cb_vs_vertexshader = &cb_vs_vertexshader;
    setPosition(0.0f, 0.0f, 0.0f);
    setRotation(0.0f, 0.0f, 0.0f);
    try {
        if (!loadModel(filePath)) {
            return false;
        }
    }
    catch (const COMException& e) {
        ErrorLogger::log(e);
        return false;
    }

    updateWorldMatrix();

    return true; 
}

void ModelClass::setTexture(ID3D11ShaderResourceView* texture) {
    this->texture = texture;
}

void ModelClass::draw(const XMMATRIX& viewProjectionMatrix) {
    cb_vs_vertexshader->data.mat = worldMatrix * viewProjectionMatrix;
    cb_vs_vertexshader->data.mat = XMMatrixTranspose(cb_vs_vertexshader->data.mat);
    cb_vs_vertexshader->applyChanges();
    deviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexshader->GetAddressOf());
    deviceContext->PSSetShaderResources(0, 1, &texture);

    for (size_t i = 0; i < meshes.size(); i++) {
        meshes[i].draw();
    }
}

void ModelClass::updateWorldMatrix() {
    worldMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * XMMatrixTranslation(pos.x, pos.y, pos.z);
    XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, rot.y, 0.0f);
    vecForward = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
    vecBackward = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
    vecLeft = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrix);
    vecRight = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
}

bool Model::ModelClass::loadModel(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

    if (pScene == nullptr) {
        return false;
    }

    processNode(pScene->mRootNode, pScene);

    return true;
}

void Model::ModelClass::processNode(aiNode* node, const aiScene* scene) {
    for (size_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (size_t i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::ModelClass::processMesh(aiMesh* mesh, const aiScene* scene) {
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

    return Mesh(device, deviceContext, vertices, indices);
}

const XMVECTOR& ModelClass::getPositionVector() const {
    return posVector;
}

const XMFLOAT3& ModelClass::getPositionFloat3() const {
    return pos;
}

const XMVECTOR& ModelClass::getRotationVector() const {
    return rotVector;
}

const XMFLOAT3& ModelClass::getRotationFloat3() const {
    return rot;
}

const XMVECTOR& ModelClass::getForwardVector() {
    return vecForward;
}

const XMVECTOR& ModelClass::getBackwardVector() {
    return vecBackward;
}

const XMVECTOR& ModelClass::getLeftVector() {
    return vecLeft;
}

const XMVECTOR& ModelClass::getRightVector() {
    return vecRight;
}

void ModelClass::setPosition(const XMVECTOR& pos) {
    XMStoreFloat3(&this->pos, pos);
    posVector = pos;
    updateWorldMatrix();
}

void ModelClass::setPosition(const XMFLOAT3& pos) {
    this->pos = pos;
    posVector = XMLoadFloat3(&this->pos);
    updateWorldMatrix();

}

void ModelClass::setPosition(float x, float y, float z) {
    pos = XMFLOAT3(x, y, z);
    posVector = XMLoadFloat3(&pos);
    updateWorldMatrix();
}

void ModelClass::adjustPosition(const XMVECTOR& pos) {
    posVector += pos;
    XMStoreFloat3(&this->pos, posVector);
    updateWorldMatrix();
}

void ModelClass::adjustPosition(const XMFLOAT3& pos) {
    this->pos.x += pos.x;
    this->pos.y += pos.y;
    this->pos.z += pos.z;
    posVector = XMLoadFloat3(&this->pos);
    this->updateWorldMatrix();
}

void ModelClass::adjustPosition(float x, float y, float z) {
    pos.x += x;
    pos.y += y;
    pos.z += z;
    posVector = XMLoadFloat3(&pos);
    updateWorldMatrix();
}

void ModelClass::setRotation(const XMVECTOR& rot) {
    rotVector = rot;
    XMStoreFloat3(&this->rot, rot);
    updateWorldMatrix();
}

void ModelClass::setRotation(const XMFLOAT3& rot) {
    this->rot = rot;
    rotVector = XMLoadFloat3(&this->rot);
    updateWorldMatrix();
}

void ModelClass::setRotation(float x, float y, float z) {
    rot = XMFLOAT3(x, y, z);
    rotVector = XMLoadFloat3(&rot);
    updateWorldMatrix();
}

void ModelClass::adjustRotation(const XMVECTOR& rot) {
    rotVector += rot;
    XMStoreFloat3(&this->rot, rotVector);
    updateWorldMatrix();
}

void ModelClass::adjustRotation(const XMFLOAT3& rot) {
    this->rot.x += rot.x;
    this->rot.y += rot.y;
    this->rot.z += rot.z;
    rotVector = XMLoadFloat3(&this->rot);
    updateWorldMatrix();
}

void ModelClass::adjustRotation(float x, float y, float z) {
    rot.x += x;
    rot.y += y;
    rot.z += z;
    rotVector = XMLoadFloat3(&this->rot);
    updateWorldMatrix();
}

void ModelClass::setLookAtPos(XMFLOAT3 lookAtPos) {
    // veirfy that look at pos is not the same as camera pos. 
    // they cannot be the same as that wouldn't make sense and would result in undefined behavior.
    if (lookAtPos.x == pos.x && lookAtPos.y == pos.y && lookAtPos.z == pos.z) {
        return;
    }

    lookAtPos.x = pos.x - lookAtPos.x;
    lookAtPos.y = pos.y - lookAtPos.y;
    lookAtPos.z = pos.z - lookAtPos.z;

    float pitch = 0.0f;
    if (lookAtPos.y != 0.0f) {
        const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
        pitch = atan(lookAtPos.y / distance);
    }

    float yaw = 0.0f;
    if (lookAtPos.x != 0.0f) {
        yaw = atan(lookAtPos.x / lookAtPos.z);
    }
    if (lookAtPos.z > 0) {
        yaw += XM_PI;
    }

    setRotation(pitch, yaw, 0.0f);

}