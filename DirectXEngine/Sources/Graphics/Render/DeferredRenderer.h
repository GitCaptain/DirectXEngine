#pragma once

#include "Renderer.h"
#include <wrl/client.h>
#include <array>
#include "../Shaders.h"

struct GBuffer final {

    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    static const int BUFFER_COUNT = 3;
    enum BUFFER_DESC{WORLD_POSITION = 0, COLOR = 1, NORMAL = 2};

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
    bool initConstantBuffers();

    VertexShader vs_geometry_pass;
    VertexShader vs_light_pass;
    PixelShader ps_geometry_pass;
    PixelShader ps_light_pass;

    ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
    ConstantBuffer<CB_PS_Phonglight> cb_ps_phonglight;
    ConstantBuffer<CB_PS_Camera> cb_ps_camera;

    ComPtr<ID3D11SamplerState> perPixelSamplerState = nullptr;

    GBuffer gbuffer;
};
