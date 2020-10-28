#pragma once

#pragma comment(lib, "D3DCompiler.lib")
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include "..\\ErrorLogger.h"

class VertexShader {
public:
    bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device,
                    std::wstring shaderpath,
                    D3D11_INPUT_ELEMENT_DESC* layoutDescription,
                    UINT numElements);
    ID3D11VertexShader* getShader();
    ID3D10Blob* getBuffer();
    ID3D11InputLayout* getInputLayout();
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader = nullptr;
    Microsoft::WRL::ComPtr<ID3D10Blob> shaderBuffer = nullptr;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout = nullptr;
};