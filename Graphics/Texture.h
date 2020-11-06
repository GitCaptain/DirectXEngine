#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <assimp/material.h>
#include "Color.h"

enum class TextureStorageType {
    Invalid,
    None,
    EmbeddedIndexCompressed,
    EmbeddedIndexNonCompressed,
    EmbeddedCompressed,
    EmbeddedNonCompressed,
    Disk,
};

class Texture {
public:
    Texture(ID3D11Device *device, const Colors::Color &color, aiTextureType type);
    Texture(ID3D11Device* device, 
            const Colors::Color *colorData, 
            UINT width,
            UINT height, 
            aiTextureType type);
    Texture(ID3D11Device* device, const std::string& filePath, aiTextureType type);

    aiTextureType getType();
    ID3D11ShaderResourceView* getTextureResourceView();
    ID3D11ShaderResourceView** getTextureResourceViewAddress();

private:
    void initialize1x1ColorTexture(ID3D11Device* device, 
                                   const Colors::Color &colorData, 
                                   aiTextureType type);
    void initializeColorTexture(ID3D11Device* device,
                                const Colors::Color* colorData,
                                UINT width,
                                UINT height,
                                aiTextureType type);
    Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView = nullptr;
    aiTextureType type = aiTextureType::aiTextureType_UNKNOWN;
};