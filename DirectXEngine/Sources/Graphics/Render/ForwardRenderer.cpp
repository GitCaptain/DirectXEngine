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
    deviceContext->PSSetConstantBuffers(0, 1, cb_ps_phonglight.GetAddressOf());
    deviceContext->PSSetConstantBuffers(1, 1, cb_ps_camera.GetAddressOf());

    deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
}

void ForwardRenderer::renderScene(App::Scene* scene, const float bgcolor[4] ) {
    deviceContext->ClearRenderTargetView(renderTargetView.Get(), bgcolor);
    deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    const std::vector<RenderableGameObject*> &renderables = scene->getRenderables();
    const DirectX::XMFLOAT3 camPos = scene->getCameraInfo().getPositionFloat3();
    // update lights
    const LightInfo& light = scene->getLightInfo();
    cb_ps_phonglight.data.ambientLightColor = light.ambient.lightColor;
    cb_ps_phonglight.data.ambientLightStrength = light.ambient.lightStrength;
    cb_ps_phonglight.data.dynamicLightColor = light.dLights[0].lightColor;
    cb_ps_phonglight.data.dynamicLightStrength = light.dLights[0].lightStrength;
    cb_ps_phonglight.data.dynamicLightPosition = light.dLights[0].getPositionFloat3();
    cb_ps_phonglight.data.dynamicLightAttenuation_a = light.dLights[0].attenuation_a;
    cb_ps_phonglight.data.dynamicLightAttenuation_b = light.dLights[0].attenuation_b;
    cb_ps_phonglight.data.dynamicLightAttenuation_c = light.dLights[0].attenuation_c;

    // Should be configured from the material
    cb_ps_phonglight.data.shinessPower = 32;
    cb_ps_phonglight.data.specularStrength = 0.5;

    cb_ps_phonglight.applyChanges();

    // update camera
    cb_ps_camera.data.cameraWorldPosition = camPos;
    cb_ps_camera.applyChanges();
    ///

    const DirectX::XMMATRIX viewProj = scene->getViewMatrix() * scene->getProjectionMatrix();
    for (RenderableGameObject* rgo : renderables) {
        const DirectX::XMMATRIX& worldMatrix = rgo->getWorldMatrix();
        const DirectX::XMMATRIX worldViewProjectionMatrix = worldMatrix * viewProj;
        const Model& model = rgo->getModel();
        const std::vector<Mesh>& meshes = model.getMeshes();
        for (const Mesh &mesh: meshes) {
            const VertexBuffer<Vertex3D>& vertexBuffer = mesh.getVertexBuffer();
            const IndexBuffer &indexBuffer = mesh.getIndexBuffer();
            const UINT offset = 0;
            const Texture* const t = mesh.getTexture();
            if (t) {
                deviceContext->PSSetShaderResources(0, 1, t->getTextureResourceViewAddress());
            }
            cb_vs_vertexshader.data.worldViewProjectionMatrix = mesh.getTransformMatrix() * worldViewProjectionMatrix;
            cb_vs_vertexshader.data.worldMatrix = mesh.getTransformMatrix() * worldMatrix;
            cb_vs_vertexshader.applyChanges();
            deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.getStridePtr(), &offset);
            deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
            deviceContext->DrawIndexed(indexBuffer.getIndexCount(), 0, 0);
        }
    }
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

bool ForwardRenderer::initConstantBuffers() {

    // initialize constants buffers
    try {
        HRESULT hr = cb_vs_vertexshader.initialize(device.Get(), deviceContext.Get());
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_vs_vertexhader constant buffer.");

        hr = cb_ps_phonglight.initialize(device.Get(), deviceContext.Get());
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_ps_phonglight constant buffer.");

        hr = cb_ps_camera.initialize(device.Get(), deviceContext.Get());
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_ps_phonglight constant buffer.");
    }
    catch (const COMException& e) {
        ErrorLogger::log(e);
        return false;
    }

    return true;
}