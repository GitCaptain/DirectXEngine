#pragma once
#include <d3d11.h>
#include "Renderer.h"

class ForwardRenderer final: public Renderer {

public:
    bool initRenderer(HWND renderWindowHandle, int windowWidth, int windowHeight) override;
    void preparePipeline() override;
    void renderScene(const App::Scene* const scene, const float bgcolor[4]) override;

private:
    bool initShaders() override;

    VertexShader vertexShader;
    PixelShader pixelShader;
};
