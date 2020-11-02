#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

template <typename T>
class VertexBuffer {

public:
    VertexBuffer() = default;
    VertexBuffer(const VertexBuffer<T>& rhs) = delete;
    ID3D11Buffer* Get() const;
    ID3D11Buffer* const * GetAddressOf() const;
    UINT getBufferSize() const;
    const UINT getStride() const;
    const UINT* getStridePtr() const;
    HRESULT initialize(ID3D11Device *device, T *data, UINT numVertices);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    std::unique_ptr<UINT> stride;
    UINT bufferSize = 0;

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
UINT VertexBuffer<T>::getBufferSize() const {
    return bufferSize;
}

template<typename T>
const UINT VertexBuffer<T>::getStride() const {
    return *stride.get();
}

template<typename T>
const UINT* VertexBuffer<T>::getStridePtr() const {
    return stride.get();
}

template<typename T>
HRESULT VertexBuffer<T>::initialize(ID3D11Device* device, T* data, UINT numVertices) {

    if (buffer.Get() != nullptr) {
        buffer.Reset();
    }

    bufferSize = numVertices;
    if (stride.get() == nullptr) {
        stride = std::make_unique<UINT>(sizeof(T));
    }
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(T) * numVertices;
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
