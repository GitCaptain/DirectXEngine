#include "IndexBuffer.h"

ID3D11Buffer* IndexBuffer::Get() const {
    return buffer.Get();
}

ID3D11Buffer* const* IndexBuffer::GetAddressOf() const {
    return buffer.GetAddressOf();
}

UINT IndexBuffer::getIndexCount() const {
    return indexCount;
}

HRESULT IndexBuffer::initialize(ID3D11Device* device, DWORD* data, UINT indexCount) {
    
    if (buffer.Get() != nullptr) {
        buffer.Reset();
    }

    this->indexCount = indexCount;
    // Load index data
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    // TODO: change if some cpu operations needed
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA indexBufferData;
    ZeroMemory(&indexBufferData, sizeof(indexBufferData));
    indexBufferData.pSysMem = data;

    HRESULT hr = device->CreateBuffer(&indexBufferDesc, &indexBufferData, buffer.GetAddressOf());
    return hr;
}
