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
    void renderScene(const App::Scene* const scene, const GraphicsSettings* gs, const float bgcolor[4]) override;

private:
    bool initShaders() override;
    void geometryPass(const App::Scene* const scene, const float bgcolor[4], const DirectX::XMMATRIX& viewProj);
    void lightPass(const App::Scene* const scene, const float bgcolor[4], const DirectX::XMMATRIX& viewProj);
    bool initVertexBuffers();
    void drawLights(
        const LightInfo& lightInfo,
        const XMMATRIX& viewProj
    );

    VertexBuffer<VertexPosition2D> light_pass_vertex_buf;
    IndexBuffer light_pass_index_buf;

    VertexShader vs_geometry_pass;
    VertexShader vs_light_pass;
    PixelShader ps_geometry_pass;
    PixelShader ps_light_pass;

    GBuffer gbuffer;
    const GraphicsSettings* gSettings = nullptr;
};
