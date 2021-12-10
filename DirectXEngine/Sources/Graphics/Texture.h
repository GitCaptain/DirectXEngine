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
    Texture(ID3D11Device *device, const Colors::Color &color, aiTextureType type, bool forceSRGB = true);
    Texture(ID3D11Device* device,
            const Colors::Color *colorData,
            UINT width,
            UINT height,
            aiTextureType type,
            bool forceSRGB = true
    );
    Texture(ID3D11Device* device, const std::string& filePath, aiTextureType type, bool forceSRGB = true);
    Texture(ID3D11Device* device, const uint8_t* pData, size_t size, aiTextureType type, bool forceSRGB = true);

    const aiTextureType getType() const;
    ID3D11ShaderResourceView* const  getTextureResourceView() const;
    ID3D11ShaderResourceView* const* getTextureResourceViewAddress() const;

private:
    void initialize1x1ColorTexture(
        ID3D11Device* device,
        const Colors::Color &colorData,
        aiTextureType type,
        bool srgb
    );
    void initializeColorTexture(
        ID3D11Device* device,
        const Colors::Color* colorData,
        UINT width,
        UINT height,
        aiTextureType type_,
        bool srgb
    );
    Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTextureView = nullptr;
    aiTextureType type = aiTextureType::aiTextureType_UNKNOWN;
};
