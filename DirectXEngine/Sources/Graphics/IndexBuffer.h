#pragma once

#include <wrl/client.h>
#include <vector>
#include <d3d11.h>

class IndexBuffer {
public:
    IndexBuffer(const IndexBuffer& rhs) = default;
    IndexBuffer() = default;
    ID3D11Buffer* Get() const;
    ID3D11Buffer* const* GetAddressOf() const;
    UINT getIndexCount() const;
    HRESULT initialize(ID3D11Device *device, DWORD *data, UINT numIndices);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    UINT indexCount = 0;
};