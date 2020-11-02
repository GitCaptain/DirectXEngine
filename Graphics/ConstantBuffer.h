#pragma once

#include <d3d11.h>
#include <WRL/client.h>
#include "..\\ErrorLogger.h"
#include "ConstantBufferTypes.h"

template <typename T>
class ConstantBuffer {

public:
    ConstantBuffer() = default;
    ConstantBuffer(const ConstantBuffer<T>& rhs) = delete;
    T data;
    ID3D11Buffer* Get() const;
    ID3D11Buffer* const* GetAddressOf() const;
    HRESULT initialize(ID3D11Device *device, ID3D11DeviceContext *deviceContext);
    bool applyChanges();

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    ID3D11DeviceContext* deviceContext = nullptr;
};

template<typename T>
inline ID3D11Buffer* ConstantBuffer<T>::Get() const {
    return buffer.Get();
}

template<typename T>
inline ID3D11Buffer* const* ConstantBuffer<T>::GetAddressOf() const {
    return buffer.GetAddressOf();
}

template<typename T>
inline HRESULT ConstantBuffer<T>::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {
    
    if (buffer.Get() != nullptr) {
        buffer.Reset();
    }

    this->deviceContext = deviceContext;
    
    D3D11_BUFFER_DESC desc;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.ByteWidth = static_cast<UINT>(sizeof(T) + 16 - (sizeof(T) % 16));
    desc.StructureByteStride = 0;

    HRESULT hr = device->CreateBuffer(&desc, 0, buffer.GetAddressOf());
    return hr;
}

template<typename T>
inline bool ConstantBuffer<T>::applyChanges() {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = deviceContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    ONFAILHRLOG(hr, "Failed to map constant buffer.", false);
    CopyMemory(mappedResource.pData, &data, sizeof(T));
    deviceContext->Unmap(buffer.Get(), 0);
    return true;
}
