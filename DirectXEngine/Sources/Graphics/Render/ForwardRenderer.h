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

    VertexShader vertexShader;
    PixelShader pixelShader;
};
