#pragma once

#include "RenderableGameObject3D.h"
#include "Shaders.h"
#include "ImGUI/ImGUIW.h"


constexpr int MAX_LIGHT_CNT = 256;
constexpr int LIGHT_TEXTURE_HEIGHT = 3;

class Light : public NGraphics::RenderableGameObject3D {
public:
    bool initialize(ID3D11Device *device, ID3D11DeviceContext *deviceContext, ConstantBuffer<CB_VS_m_world_viewprojeciton> &cb_vs_vertexshader);
    
    void prepare(ID3D11DeviceContext* deviceContext) override;
    void draw(const DirectX::XMMATRIX& viewProjectionMatrix) override;

    DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    float lightStrength = 1.0f;

    // ligth strength equation: strength = 1/(A + Bx +Cx*x)
    float attenuation_a = 1.0f;
    float attenuation_b = 0.1f;
    float attenuation_c = 0.1f;
    int lightsCnt = 1;
private:
    bool initLight(ID3D11Device* device);
    bool updateLight(ID3D11DeviceContext* deviceContext);
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStateLightInfo;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> lightInfo;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> lightTexture;
    // texture size is 3 * lightsCnt
    // zero row: TextureWidth(max lights cnt) +  lightsCnt, ambientLightColor, ambientLightStrength, attenuations
    // first row: dynamicLightColor + Strength
    // second row: dynamicLightPosition
    // Bind this to 10'th registers to avoid intersections
    DirectX::XMFLOAT4 lightData[LIGHT_TEXTURE_HEIGHT][MAX_LIGHT_CNT];
};