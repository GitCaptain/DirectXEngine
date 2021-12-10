#include "DeferredRenderer.h"

using NGameObject::RenderableGameObject;
using NModel::Model;

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
    inited &= initConstantBuffers();
    inited &= initVertexBuffers();
    inited &= gbuffer.initialize(graphicsState);
    return inited;
}

bool DeferredRenderer::initShaders() {

    D3D11_INPUT_ELEMENT_DESC layout_geometry_pass[] = {
        {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    UINT numElements = ARRAYSIZE(layout_geometry_pass);

    if (!vs_geometry_pass.initialize(device.Get(), shaderFolder + L"vertexshader.cso", layout_geometry_pass, numElements)) {
        ErrorLogger::log("Can't initialize geometry pass vertex shader for deferred renderer");
        return false;
    }

    if (!ps_geometry_pass.initialize(device.Get(), shaderFolder + L"deferred_geometry_pass_ps.cso")) {
        ErrorLogger::log("Can't initialize geometry pass pixel shader for deferred renderer");
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC layout_light_pass[] = {
        {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    numElements = ARRAYSIZE(layout_light_pass);

    if (!vs_light_pass.initialize(device.Get(), shaderFolder + L"deferred_light_pass_vs.cso", layout_light_pass, numElements)) {
        ErrorLogger::log("Can't initialize light pass vertex shader for deferred renderer");
        return false;
    }

    if (!ps_light_pass.initialize(device.Get(), shaderFolder + L"deferred_light_pass_ps.cso")) {
        ErrorLogger::log("Can't initialize light pass pixel shader for deferred renderer");
        return false;
    }

    return true;
}

void DeferredRenderer::renderScene(const App::Scene* const scene, const GraphicsSettings* gs, const float bgcolor[4]) {
    const DirectX::XMMATRIX viewProj = scene->getViewMatrix() * scene->getProjectionMatrix();
    gSettings = gs;
    geometryPass(scene, bgcolor, viewProj);
    lightPass(scene, bgcolor, viewProj);
}

void DeferredRenderer::geometryPass(const App::Scene* const scene, const float bgcolor[4], const DirectX::XMMATRIX& viewProj) {

    // preparation
    constexpr int sz = GBuffer::BUFFER_COUNT;
    ID3D11RenderTargetView* rtv[sz];
    for (int i = 0; i < sz; i++) {
        rtv[i] = gbuffer.renderTargetViews[i].Get();
        deviceContext->ClearRenderTargetView(rtv[i], bgcolor);
    }

    // geometry pass pipeline
    deviceContext->IASetInputLayout(vs_geometry_pass.getInputLayout());
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    deviceContext->RSSetState(rasterizerState.Get());

    deviceContext->VSSetShader(vs_geometry_pass.getShader(), nullptr, 0);
    deviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexshader.GetAddressOf());

    deviceContext->PSSetShader(ps_geometry_pass.getShader(), nullptr, 0);
    deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());

    deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
    deviceContext->OMSetRenderTargets(sz, rtv, depthStencilView.Get());

    deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    const std::vector<const RenderableGameObject*>& renderables = scene->getRenderables();
    draw(renderables, viewProj);

}

void DeferredRenderer::lightPass(const App::Scene* const scene, const float bgcolor[4], const DirectX::XMMATRIX& viewProj) {

    // preparation
    constexpr int sz = GBuffer::BUFFER_COUNT;
    ID3D11ShaderResourceView* srv[sz];
    for (int i = 0; i < sz; i++) {
        srv[i] = gbuffer.shaderResourceViews[i].Get();
    }

    // lightning pass pipeline
    deviceContext->IASetInputLayout(vs_light_pass.getInputLayout());

    deviceContext->VSSetShader(vs_light_pass.getShader(), nullptr, 0);

    deviceContext->PSSetShader(ps_light_pass.getShader(), nullptr, 0);
    deviceContext->PSSetConstantBuffers(0, 1, cb_ps_ambientlight.GetAddressOf());
    deviceContext->PSSetConstantBuffers(2, 1, cb_ps_camera.GetAddressOf());
    deviceContext->PSSetConstantBuffers(3, 1, cb_ps_lightsCount.GetAddressOf());
    deviceContext->PSSetConstantBuffers(4, 1, cb_ps_graphicsSettings.GetAddressOf());

    // do not use DepthStencil buffer, we already render only what we need to the texture
    deviceContext->OMSetDepthStencilState(nullptr, 0);
    // Remove 2 previous render targets
    ID3D11RenderTargetView* views[sz] = { renderTargetView.Get(), nullptr, nullptr };
    deviceContext->OMSetRenderTargets(sz, views, nullptr);
    // set gbuffer textures as current pass resource
    // only after unbinding it from previous pass render targets
    deviceContext->PSSetShaderResources(0, sz, srv);
    deviceContext->ClearRenderTargetView(renderTargetView.Get(), bgcolor);

    // update camera
    const DirectX::XMFLOAT3 camPos = scene->getCameraInfo().getPositionFloat3();
    cb_ps_camera.data.cameraWorldPosition = camPos;
    cb_ps_camera.applyChanges();
    ///

    cb_ps_graphicsSettings.data.gamma = gSettings->gammaCoef;
    cb_ps_graphicsSettings.applyChanges();

    const LightInfo& lightInfo = scene->getLightInfo();
    drawLights(lightInfo, viewProj);
}

bool DeferredRenderer::initVertexBuffers() {

    constexpr size_t vcnt = 6;
    VertexPosition3D vp[vcnt] = {
        {-1, -1},
        {-1, 1},
        {1, 1},
        {1, 1},
        {1, -1},
        {-1, -1}
    };
    HRESULT hr = light_pass_vertex_buf.initialize(device.Get(), vp, vcnt);
    COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for light pass in deferred renderer");

    return true;
}

void DeferredRenderer::drawLights(const LightInfo& lightInfo, const XMMATRIX& viewProj) {
    prepareLights(lightInfo, 3);
    const UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, light_pass_vertex_buf.GetAddressOf(), light_pass_vertex_buf.getStridePtr(), &offset);
    deviceContext->Draw(light_pass_vertex_buf.getVertexCount(), 0);
}
