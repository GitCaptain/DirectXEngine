#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

template <typename T>
class VertexBuffer {

public:
    VertexBuffer() = default;
    VertexBuffer(const VertexBuffer<T>& rhs) = default;
    ID3D11Buffer* Get() const;
    ID3D11Buffer* const* GetAddressOf() const;
    UINT getVertexCount() const;
    const UINT getStride() const;
    const UINT* getStridePtr() const;
    HRESULT initialize(ID3D11Device* device, T* data, UINT numVertices);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    UINT stride = sizeof(T);
    UINT vertexCount = 0;

};

template<typename T>
ID3D11Buffer* VertexBuffer<T>::Get() const {
    return buffer.Get();
}

template<typename T>
ID3D11Buffer* const* VertexBuffer<T>::GetAddressOf() const {
    return buffer.GetAddressOf();
}

template<typename T>
UINT VertexBuffer<T>::getVertexCount() const {
    return vertexCount;
}

template<typename T>
const UINT VertexBuffer<T>::getStride() const {
    return stride;
}

template<typename T>
const UINT* VertexBuffer<T>::getStridePtr() const {
    return &stride;
}

template<typename T>
HRESULT VertexBuffer<T>::initialize(ID3D11Device* device, T* data, UINT vertexCount) {

    if (buffer.Get() != nullptr) {
        buffer.Reset();
    }

    this->vertexCount = vertexCount;

    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = stride * vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    // TODO: change if some cpu operations needed
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData;
    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));

    vertexBufferData.pSysMem = data;

    HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, buffer.GetAddressOf());
    return hr;
}
