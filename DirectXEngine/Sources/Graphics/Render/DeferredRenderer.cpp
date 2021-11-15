#include "DeferredRenderer.h"

bool GBuffer::initialize(GraphicsState &gs) {

    DXGI_FORMAT textureFormat = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
    try {
        // Create the render target textures.
        for (auto& targetTexture : renderTargetTextures) {
            Renderer::createTexture(gs.device, &targetTexture, gs.windowWidth, gs.windowHeight, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, textureFormat);
        }

        CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{ D3D11_RTV_DIMENSION_TEXTURE2D , textureFormat, 0 };
        // Create the render target views.
        for (int i = 0; i < BUFFER_COUNT; i++) {
            Renderer::createRenderTargetView(gs.device, renderTargetTextures[i].Get(), &renderTargetViews[i], &renderTargetViewDesc);
        }

        CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{ D3D11_SRV_DIMENSION_TEXTURE2D, textureFormat, 0, 1 };
        // Create the shader resource views.
        for (int i = 0; i < BUFFER_COUNT; i++) {
            Renderer::createShaderResourceView(gs.device, renderTargetTextures[i].Get(), &shaderResourceViews[i], &shaderResourceViewDesc);
        }
    }
    catch (const COMException& e) {
        ErrorLogger::log(e);
        return false;
    }

    return true;
}

bool DeferredRenderer::initRenderer(HWND renderWindowHandle, int windowWidth, int windowHeight) {
    bool inited = Renderer::initRenderer(renderWindowHandle, windowWidth, windowHeight);
    inited &= initShaders();
    inited &= gbuffer.initialize(graphicsState);
    return inited;
}

void DeferredRenderer::preparePipeline() {
}

void DeferredRenderer::renderScene(App::Scene* scene, const float bgcolor[4]) {

    deviceContext->IASetInputLayout(vs_geometry_pass.getInputLayout());
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->RSSetState(rasterizerState.Get());
    deviceContext->VSSetShader(vs_geometry_pass.getShader(), nullptr, 0);
    deviceContext->PSSetShader(ps_geometry_pass.getShader(), nullptr, 0);
    deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
    deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);

    constexpr int sz = GBuffer::BUFFER_COUNT;
    ID3D11RenderTargetView* rtv[sz];
    for (int i = 0; i < sz; i++) {
        rtv[i] = gbuffer.renderTargetViews[i].Get();
    }
    for (auto &rtv: gbuffer.renderTargetViews) {
        deviceContext->ClearRenderTargetView(rtv.Get(), bgcolor);
    }
    deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    deviceContext->OMSetRenderTargets(sz, rtv, depthStencilView.Get());

    scene->render();
}

bool DeferredRenderer::initShaders() {

    D3D11_INPUT_ELEMENT_DESC layout_geometry_pass[] = {
        {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    UINT numElements = ARRAYSIZE(layout_geometry_pass);

    if (!vs_geometry_pass.initialize(device.Get(), shaderFolder + L"vertexshader.cso", layout_geometry_pass, numElements)) {
        return false;
    }

    if (!ps_geometry_pass.initialize(device.Get(), shaderFolder + L"deferred_geometry_pass_ps.cso")) {
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC layout_light_pass[] = {
        {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    UINT numElements = ARRAYSIZE(layout_light_pass);

    if (!vs_light_pass.initialize(device.Get(), shaderFolder + L"deferred_light_pass_vs.cso", layout_light_pass, numElements)) {
        return false;
    }

    if (!ps_light_pass.initialize(device.Get(), shaderFolder + L"deferred_light_pass_ps.cso")) {
        return false;
    }

}
