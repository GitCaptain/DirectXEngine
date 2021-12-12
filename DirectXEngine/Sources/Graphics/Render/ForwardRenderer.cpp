#include "ForwardRenderer.h"
#include <vector>

using NGameObject::RenderableGameObject;
using NModel::Model;

bool ForwardRenderer::initRenderer(HWND renderWindowHandle, int windowWidth, int windowHeight) {
    bool inited = Renderer::initRenderer(renderWindowHandle, windowWidth, windowHeight);
    inited &= initShaders();
    inited &= initConstantBuffers();
    return inited;
}

void ForwardRenderer::preparePipeline() {
    Renderer::preparePipeline();
    deviceContext->IASetInputLayout(vertexShader.getInputLayout());
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    deviceContext->RSSetState(rasterizerState.Get());

    deviceContext->VSSetShader(vertexShader.getShader(), nullptr, 0);
    deviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexshader.GetAddressOf());

    deviceContext->PSSetShader(pixelShader.getShader(), nullptr, 0);
    deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
    deviceContext->PSSetConstantBuffers(0, 1, cb_ps_ambientlight.GetAddressOf());
    deviceContext->PSSetConstantBuffers(2, 1, cb_ps_camera.GetAddressOf());
    deviceContext->PSSetConstantBuffers(3, 1, cb_ps_lightsCount.GetAddressOf());
    deviceContext->PSSetConstantBuffers(4, 1, cb_ps_graphicsSettings.GetAddressOf());

    deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
}

void ForwardRenderer::renderScene(const App::Scene* const scene, const GraphicsSettings* gs, const float bgcolor[4] ) {
    deviceContext->ClearRenderTargetView(renderTargetView.Get(), bgcolor);
    deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    const LightInfo& light = scene->getLightInfo();
    prepareLights(light, 2);

    // update camera
    const DirectX::XMFLOAT3 camPos = scene->getCameraInfo().getPositionFloat3();
    cb_ps_camera.data.cameraWorldPosition = camPos;
    cb_ps_camera.applyChanges();

    applyGraphicsSettings(gs);

    const std::vector<const RenderableGameObject*>& renderables = scene->getRenderables();
    const DirectX::XMMATRIX viewProj = scene->getViewMatrix() * scene->getProjectionMatrix();
    draw(renderables, viewProj);
}

bool ForwardRenderer::initShaders() {

    // 3d shaders
    D3D11_INPUT_ELEMENT_DESC layout3D[] = {
        {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    UINT numElements = ARRAYSIZE(layout3D);

    if (!vertexShader.initialize(device.Get(), shaderFolder + L"vertexshader.cso", layout3D, numElements)) {
        return false;
    }

    if (!pixelShader.initialize(device.Get(), shaderFolder + L"PhongLightning_ps.cso")) {
        return false;
    }

    return true;
}
