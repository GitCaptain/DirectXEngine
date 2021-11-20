#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include "Renderer.h"
#include "../Shaders.h"

class ForwardRenderer final: public Renderer {

public:
    bool initRenderer(HWND renderWindowHandle, int windowWidth, int windowHeight) override;
    void preparePipeline() override;
    void renderScene(App::Scene* scene, const float bgcolor[4]) override;

private:
    bool initShaders();
    bool initConstantBuffers();

    VertexShader vertexShader;
    PixelShader pixelShader;
    ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
    ConstantBuffer<CB_PS_Phonglight> cb_ps_phonglight;
    ConstantBuffer<CB_PS_Camera> cb_ps_camera;
};
