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
    inited &= gbuffer.initialize(graphicsState);
    return inited;
}

void DeferredRenderer::preparePipeline() { }

void DeferredRenderer::renderScene(App::Scene* scene, const float bgcolor[4]) {

    // preparation
    constexpr int sz = GBuffer::BUFFER_COUNT;
    ID3D11RenderTargetView* rtv[sz];
    ID3D11ShaderResourceView* srv[sz];
    for (int i = 0; i < sz; i++) {
        rtv[i] = gbuffer.renderTargetViews[i].Get();
        srv[i] = gbuffer.shaderResourceViews[i].Get();
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

    for (auto &rtv: gbuffer.renderTargetViews) {
        deviceContext->ClearRenderTargetView(rtv.Get(), bgcolor);
    }
    deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    const std::vector<RenderableGameObject*>& renderables = scene->getRenderables();
    const DirectX::XMMATRIX viewProj = scene->getViewMatrix() * scene->getProjectionMatrix();
    for (RenderableGameObject* rgo : renderables) {
        const DirectX::XMMATRIX& worldMatrix = rgo->getWorldMatrix();
        const DirectX::XMMATRIX worldViewProjectionMatrix = worldMatrix * viewProj;
        const Model& model = rgo->getModel();
        const std::vector<Mesh>& meshes = model.getMeshes();
        for (const Mesh& mesh : meshes) {
            const VertexBuffer<Vertex3D>& vertexBuffer = mesh.getVertexBuffer();
            const IndexBuffer& indexBuffer = mesh.getIndexBuffer();
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
    const DirectX::XMFLOAT3 camPos = scene->getCameraInfo().getPositionFloat3();
    cb_ps_camera.data.cameraWorldPosition = camPos;
    cb_ps_camera.applyChanges();
    ///

    // lightning pass pipeline
    deviceContext->IASetInputLayout(vs_light_pass.getInputLayout());

    deviceContext->VSSetShader(vs_light_pass.getShader(), nullptr, 0);

    deviceContext->PSSetShader(ps_light_pass.getShader(), nullptr, 0);
    deviceContext->PSSetShaderResources(0, 3, srv);
    deviceContext->PSSetConstantBuffers(0, 1, cb_ps_phonglight.GetAddressOf());
    deviceContext->PSSetConstantBuffers(1, 1, cb_ps_camera.GetAddressOf());
    deviceContext->PSSetSamplers(0, 1, perPixelSamplerState.GetAddressOf());

    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

    deviceContext->ClearRenderTargetView(renderTargetView.Get(), bgcolor);

    for (RenderableGameObject* rgo : renderables) {
        const DirectX::XMMATRIX& worldMatrix = rgo->getWorldMatrix();
        const DirectX::XMMATRIX worldViewProjectionMatrix = worldMatrix * viewProj;
        const Model& model = rgo->getModel();
        const std::vector<Mesh>& meshes = model.getMeshes();
        for (const Mesh& mesh : meshes) {
            const VertexBuffer<Vertex3D>& vertexBuffer = mesh.getVertexBuffer();
            const IndexBuffer& indexBuffer = mesh.getIndexBuffer();
            const UINT offset = 0;
            cb_vs_vertexshader.data.worldViewProjectionMatrix = mesh.getTransformMatrix() * worldViewProjectionMatrix;
            cb_vs_vertexshader.applyChanges();
            deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.getStridePtr(), &offset);
            deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
            deviceContext->DrawIndexed(indexBuffer.getIndexCount(), 0, 0);
        }
    }
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

    if (!vs_light_pass.initialize(device.Get(), shaderFolder + L"deferred_light_pass_vs.cso", layout_geometry_pass, numElements)) {
        ErrorLogger::log("Can't initialize light pass vertex shader for deferred renderer");
        return false;
    }

    if (!ps_light_pass.initialize(device.Get(), shaderFolder + L"deferred_light_pass_ps.cso")) {
        ErrorLogger::log("Can't initialize light pass pixel shader for deferred renderer");
        return false;
    }

    return true;
}

void DeferredRenderer::createSamplerState() {
    Renderer::createSamplerState();
    // Create sampler description for sampler state
    CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    // Create sampler state
    HRESULT hr = device->CreateSamplerState(&sampDesc, &perPixelSamplerState);
    COM_ERROR_IF_FAILED(hr, "Failed to create per pixel sampler state.");
}

bool DeferredRenderer::initConstantBuffers() {
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
