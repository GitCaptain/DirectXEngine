#pragma once

#pragma comment(lib, "D3DCompiler.lib")
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include "..\\ErrorLogger.h"

class VertexShader {
public:
    bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath);
    ID3D11VertexShader* getShader();
    ID3D10Blob* getBuffer();
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader = nullptr;
    Microsoft::WRL::ComPtr<ID3D10Blob> shaderBuffer = nullptr;
};