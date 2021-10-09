#include "PongScene.h"

#ifdef ENABLE_IMGUI
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#endif

using namespace App;

bool PongScene::initialize(GraphicsState& graphicsState) {

    assert(graphicsState.device && "device is nullptr");
    assert(graphicsState.deviceContext && "deviceContext is nullptr");

    this->graphicsState = &graphicsState;

    if (!initializeShaders()) {
        return false;
    }

    if (!ball.initialize("Resources\\Objects\\sphere.fbx", graphicsState.device, graphicsState.deviceContext, cb_vs_vertexshader)) {
        return false;
    }
    if (!border.initialize("Resources\\Objects\\cube3d.fbx", graphicsState.device, graphicsState.deviceContext, cb_vs_vertexshader)) {
        return false;
    }
    camera.setPosition(0.0f, 0.0f, -2.0f);
    camera.setProjectionValues(90.0f, static_cast<float>(*graphicsState.windowWidth) / static_cast<float>(*graphicsState.windowHeight), 0.1f, 3000.0f);
    return true;
}

void PongScene::render() {

    graphicsState->deviceContext->IASetInputLayout(vertexShader.getInputLayout());
    graphicsState->deviceContext->VSSetShader(vertexShader.getShader(), nullptr, 0);
    graphicsState->deviceContext->PSSetShader(pixelShader.getShader(), nullptr, 0);

    cb_ps_light.data.dynamicLightColor = light.lightColor;
    cb_ps_light.data.dynamicLightStrength = light.lightStrength;
    cb_ps_light.data.dynamicLightPosition = light.getPositionFloat3();
    cb_ps_light.data.dynamicLightAttenuation_a = light.attenuation_a;
    cb_ps_light.data.dynamicLightAttenuation_b = light.attenuation_b;
    cb_ps_light.data.dynamicLightAttenuation_c = light.attenuation_c;

    cb_ps_light.applyChanges();
    graphicsState->deviceContext->PSSetConstantBuffers(0, 1, cb_ps_light.GetAddressOf());

#ifdef ENABLE_IMGUI
    // start dear imgui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    // create imgui test window
    ImGui::Begin("Light Controls");
    ImGui::DragFloat3("Ambient light color", &cb_ps_light.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Ambient light strength", &cb_ps_light.data.ambientLightStrength, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Dynamic light Attenuation A", &light.attenuation_a, 0.01f, 0.1f, 1.0f);
    ImGui::DragFloat("Dynamic light Attenuation B", &light.attenuation_b, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Dynamic light Attenuation C", &light.attenuation_c, 0.01f, 0.0f, 1.0f);
    ImGui::End();
    // Assemble Together Draw data
    ImGui::Render();
    // render draw data
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

    //border.setScale(1.f, 0.1f, 2.f);
    border.draw(camera.getViewMatrix() * camera.getProjectionMatrix());
}

void App::PongScene::update(HID::Keyboard& kbd, HID::Mouse& mouse, float dt) {

    while (!kbd.isCharBufferEmpty()) {
        auto ch = kbd.readChar();
    }

    while (!kbd.isKeyBufferEmpty()) {
        auto event = kbd.readKey();
        auto keycode = event.getKeyCode();
    }

    while (!mouse.isEventBufferEmpty()) {
        auto me = mouse.readEvent();
        if (mouse.isRightDown()) {
            if (me.getType() == HID::MouseEvent::EventType::RAW_MOVE) {
                camera.adjustRotation(me.getPosY() * 0.01f, me.getPosX() * 0.01f, 0.0f);
            }
        }
    }

    const float camera3DSpeed = 0.005f;
    float cameraSpeedMultiplyer = 1.0f;

    if (kbd.isKeyPressed(VK_SHIFT)) {
        cameraSpeedMultiplyer *= 50;
        dt *= 50;
    }

    if (kbd.isKeyPressed('W')) {
        camera.adjustPosition(camera.getForwardVector() * camera3DSpeed * cameraSpeedMultiplyer * dt);
    }
    if (kbd.isKeyPressed('S')) {
        camera.adjustPosition(camera.getBackwardVector() * camera3DSpeed * dt);
    }
    if (kbd.isKeyPressed('A')) {
        camera.adjustPosition(camera.getLeftVector() * camera3DSpeed * dt);
    }
    if (kbd.isKeyPressed('D')) {
        camera.adjustPosition(camera.getRightVector() * camera3DSpeed * dt);
    }
    if (kbd.isKeyPressed(VK_SPACE)) {
        camera.adjustPosition(0.0f, camera3DSpeed * dt, 0.0f);
    }
    if (kbd.isKeyPressed(VK_CONTROL)) {
        camera.adjustPosition(0.0f, - camera3DSpeed * dt, 0.0f);
    }

    if (kbd.isKeyPressed('C')) {
        DirectX::XMVECTOR lightPosition = camera.getPositionVector();
        lightPosition += camera.getForwardVector();
        light.setPosition(lightPosition);
        light.setRotation(camera.getRotationFloat3());
    }
}

bool PongScene::initializeShaders() {

    std::wstring shaderFolder = L"";

#pragma region DetermineShaderPath
    if (IsDebuggerPresent() == TRUE) {
#ifdef _DEBUG
#ifdef _WIN64
        shaderFolder = L"x64\\Debug\\";
#else
        shaderFolder = L"x86\\Debug\\";
#endif
#else
#ifdef _WIN64
        shaderFolder = L"x64\\Release\\";
#else
        shaderFolder = L"x86\\Release\\";
#endif
#endif
    }

    // 3d shaders
    D3D11_INPUT_ELEMENT_DESC layout3D[] = {
        {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    UINT numElements = ARRAYSIZE(layout3D);

    if (!vertexShader.initialize(graphicsState->device, shaderFolder + L"vertexshader.cso", layout3D, numElements)) {
        return false;
    }

    if (!pixelShader.initialize(graphicsState->device, shaderFolder + L"pixelshader.cso")) {
        return false;
    }

    // initialize constants buffers
    try {

        HRESULT hr = cb_vs_vertexshader.initialize(graphicsState->device, graphicsState->deviceContext);
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_vs_vertexhader constant buffer.");

        hr = cb_ps_light.initialize(graphicsState->device, graphicsState->deviceContext);
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_ps_light constant buffer.");
    }
    catch (const COMException& e) {
        ErrorLogger::log(e);
        return false;
    }

    cb_ps_light.data.ambientLightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    cb_ps_light.data.ambientLightStrength = 1.0f;

    if (!light.initialize(graphicsState->device, graphicsState->deviceContext, cb_vs_vertexshader)) {
        return false;
    }

    return true;
}
