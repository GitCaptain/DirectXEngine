#pragma once
#include "RenderableGameObject.h"
#include <wrl/client.h>
#include "GraphicsState.h"

struct PointLight final : public NGameObject::RenderableGameObject {
    DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    DirectX::XMFLOAT3 attenuations = DirectX::XMFLOAT3(1.0f, 0.1f, 0.1f);
    float lightStrength = 1.0f;
};

struct AmbientLight final {
    DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    float lightStrength = 1.0f;
};

class LightInfo final {

    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    ComPtr<ID3D11Texture1D> lightTex = nullptr;
    ComPtr<ID3D11ShaderResourceView> lightView = nullptr;
    static constexpr size_t FLOATS_PER_DATA_ELEMENT = 10;
    mutable std::vector<float> lightData;

public:
    bool initialize(GraphicsState& state, size_t max_light_cnt);
    ID3D11ShaderResourceView* const* getLightTextureSRV(GraphicsState& state) const;
    int getLightsCnt() const noexcept;
    AmbientLight ambient;
    std::vector<PointLight> pointLights;
};
