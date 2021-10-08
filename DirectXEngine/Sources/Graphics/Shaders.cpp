#include "Shaders.h"

bool VertexShader::initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device,
                              std::wstring shaderpath, 
                              D3D11_INPUT_ELEMENT_DESC* layoutDescription, 
                              UINT numElements) {
    
    HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), shaderBuffer.GetAddressOf());
    
    if (FAILED(hr)) {
        std::wstring errorMsg = L"Failed to load shader: ";
        errorMsg += shaderpath;
        ErrorLogger::log(hr, errorMsg);
        return false;
    }

    hr = device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr, shader.GetAddressOf());

    if (FAILED(hr)) {
        std::wstring errorMsg = L"Failed to create vertex shader: ";
        errorMsg += shaderpath;
        ErrorLogger::log(hr, errorMsg);
        return false;
    }


    hr = device->CreateInputLayout(layoutDescription,
                                   numElements,
                                   shaderBuffer->GetBufferPointer() ,
                                   shaderBuffer->GetBufferSize(),
                                   inputLayout.GetAddressOf());

    if (FAILED(hr)) {
        ErrorLogger::log(hr, "Failed to create input layout.");
        return false;
    }


    return true;
}

ID3D11VertexShader* VertexShader::getShader() {
    return shader.Get();
}

ID3D10Blob* VertexShader::getBuffer() {
    return shaderBuffer.Get();
}

ID3D11InputLayout* VertexShader::getInputLayout() {
    return inputLayout.Get();
}

bool PixelShader::initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath) {
    HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), shaderBuffer.GetAddressOf());

    if (FAILED(hr)) {
        std::wstring errorMsg = L"Failed to load shader: ";
        errorMsg += shaderpath;
        ErrorLogger::log(hr, errorMsg);
        return false;
    }

    hr = device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr, shader.GetAddressOf());

    if (FAILED(hr)) {
        std::wstring errorMsg = L"Failed to create pixel shader: ";
        errorMsg += shaderpath;
        ErrorLogger::log(hr, errorMsg);
        return false;
    }

    return true;
}

ID3D11PixelShader* PixelShader::getShader() {
    return shader.Get();
}

ID3D10Blob* PixelShader::getBuffer() {
    return shaderBuffer.Get();
}
