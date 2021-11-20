#pragma once

#include "Renderer.h"
#include <wrl/client.h>
#include <array>
#include "../Shaders.h"

struct GBuffer final {

    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    static const int BUFFER_COUNT = 4;
    enum BUFFER_DESC{WORLD_POSITION = 0, COLOR = 1, NORMAL = 2, TEXCOORD = 3};

    std::array<ComPtr<ID3D11Texture2D>, BUFFER_COUNT> renderTargetTextures{};
    std::array<ComPtr<ID3D11RenderTargetView>, BUFFER_COUNT> renderTargetViews{};
    std::array<ComPtr<ID3D11ShaderResourceView>, BUFFER_COUNT> shaderResourceViews{};

    bool initialize(GraphicsState& gs);

};

class DeferredRenderer final : public Renderer {

public:
    bool initRenderer(HWND renderWindowHandle, int windowWidth, int windowHeight) override;

    void preparePipeline() override;
    void renderScene(App::Scene* scene, const float bgcolor[4]) override;

private:
    bool initShaders();
    void createSamplerState() override;

    VertexShader vs_geometry_pass;
    VertexShader vs_light_pass;
    PixelShader ps_geometry_pass;
    PixelShader ps_light_pass;

    ComPtr<ID3D11SamplerState> perPixelSamplerState = nullptr;

    GBuffer gbuffer;
};
