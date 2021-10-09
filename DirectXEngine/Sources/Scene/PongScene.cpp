#include "PongScene.h"

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

    imgui = ImGUIWInstance::getPInstance();
    camera.setPosition(0.0f, tableHeight + 20, -2.0f);
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

    XMMATRIX viewProjectionMatrix = camera.getViewMatrix() * camera.getProjectionMatrix();

    // draw table
    border.setRotation(0, 0, 0);
    border.setPosition(tablePos);
    border.setScale(tableWidth, tableHeight, tableLength);
    border.draw(viewProjectionMatrix);

    // draw left border
    border.setPosition(leftBorderPos);
    border.setScale(borderWidth, borderHeight, borderLength);
    border.draw(viewProjectionMatrix);

    // draw right border
    border.setPosition(rightBorderPos);
    border.setScale(borderWidth, borderHeight, borderLength);
    border.draw(viewProjectionMatrix);

    const float pi = 3.1415926;

    // draw player pad
    border.setPosition(PlayerPos);
    border.setRotation(0, pi, 0);
    border.setScale(padWidth, padHeight, padLength);
    border.draw(viewProjectionMatrix);

    // draw AI pad
    border.setPosition(AIPos);
    border.setRotation(0, -pi , 0);
    border.setScale(padWidth, padHeight, padLength);
    border.draw(viewProjectionMatrix);

    // draw ball
    ball.setPosition(ballPosition);
    ball.setScale(ballRadius, ballRadius, ballRadius);
    ball.draw(viewProjectionMatrix);

    //light.draw(viewProjectionMatrix);


#pragma region IMGUI drawing
    imgui->startFrame();

    imgui->newWindow("Light controls")
        .attach<IMGUIFN::DRAGFLOAT3>("Ambient light color", &cb_ps_light.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f)
        .attach<IMGUIFN::DRAGFLOAT>("Ambient light strength", &cb_ps_light.data.ambientLightStrength, 0.01f, 0.0f, 1.0f)
        .attach<IMGUIFN::DRAGFLOAT>("Dynamic light Attenuation A", &light.attenuation_a, 0.01f, 0.1f, 1.0f)
        .attach<IMGUIFN::DRAGFLOAT>("Dynamic light Attenuation B", &light.attenuation_b, 0.01f, 0.0f, 1.0f)
        .attach<IMGUIFN::DRAGFLOAT>("Dynamic light Attenuation C", &light.attenuation_c, 0.01f, 0.0f, 1.0f)
        .end();

    imgui->newWindow("Camera controls")
        .attach<IMGUIFN::DRAGFLOAT>("Camera speed", &cameraSpeed, 0.005f, 0.0f, 0.1f)
        .attach<IMGUIFN::TEXT>("position: %.3f %.3f %.3f", camera.getPositionFloat3().x, camera.getPositionFloat3().y, camera.getPositionFloat3().z)
        .end();
    imgui->newWindow("table controls")
        .attach<IMGUIFN::DRAGFLOAT3>("position", &tablePos.x, 1.f, 0.0f, 100.f)
        .attach<IMGUIFN::DRAGFLOAT>("width", &tableWidth, 1.f, 0.0f, 100.f)
        .attach<IMGUIFN::DRAGFLOAT>("height", &tableHeight, 1.f, 0.0f, 100.f)
        .attach<IMGUIFN::DRAGFLOAT>("length", &tableLength, 1.f, 0.0f, 100.f)
        .end();
    imgui->newWindow("left border controls")
        .attach<IMGUIFN::DRAGFLOAT3>("position", &leftBorderPos.x, 1.f, 0.0f, 100.f)
        .attach<IMGUIFN::DRAGFLOAT>("width", &borderWidth, 1.f, 0.0f, 100.f)
        .attach<IMGUIFN::DRAGFLOAT>("height", &borderHeight, 1.f, 0.0f, 100.f)
        .attach<IMGUIFN::DRAGFLOAT>("length", &borderLength, 1.f, 0.0f, 100.f)
        .end();
    imgui->newWindow("right border controls")
        .attach<IMGUIFN::DRAGFLOAT3>("position", &rightBorderPos.x, 1, 0.0f, 100.f)
        .attach<IMGUIFN::DRAGFLOAT>("width", &borderWidth, 1.f, 0.0f, 100.f)
        .attach<IMGUIFN::DRAGFLOAT>("height", &borderHeight, 1.f, 0.0f, 100.f)
        .attach<IMGUIFN::DRAGFLOAT>("length", &borderLength, 1.f, 0.0f, 100.f)
        .end();
    imgui->newWindow("player pad controls")
        .attach<IMGUIFN::DRAGFLOAT3>("position", &PlayerPos.x, 1, -200.f, 200.f)
        .attach<IMGUIFN::DRAGFLOAT>("width", &padWidth, 1.f, 0.0f, 100.f)
        .attach<IMGUIFN::DRAGFLOAT>("height", &padHeight, 1.f, 0.0f, 100.f)
        .attach<IMGUIFN::DRAGFLOAT>("length", &padLength, 1.f, 0.0f, 100.f)
        .end();
    imgui->newWindow("AI pad controls")
        .attach<IMGUIFN::DRAGFLOAT3>("position", &AIPos.x, 1, -200.f, 200.f)
        .attach<IMGUIFN::DRAGFLOAT>("width", &padWidth, 1.f, 0.0f, 100.f)
        .attach<IMGUIFN::DRAGFLOAT>("height", &padHeight, 1.f, 0.0f, 100.f)
        .attach<IMGUIFN::DRAGFLOAT>("length", &padLength, 1.f, 0.0f, 100.f)
        .end();

    imgui->endFrame();
#pragma endregion
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

    float cameraSpeedMultiplyer = 1.0f;

    if (kbd.isKeyPressed(VK_SHIFT)) {
        cameraSpeedMultiplyer *= 50;
        dt *= 50;
    }

    if (kbd.isKeyPressed('W')) {
        camera.adjustPosition(camera.getForwardVector() * cameraSpeed * cameraSpeedMultiplyer * dt);
    }
    if (kbd.isKeyPressed('S')) {
        camera.adjustPosition(camera.getBackwardVector() * cameraSpeed * dt);
    }
    if (kbd.isKeyPressed('A')) {
        camera.adjustPosition(camera.getLeftVector() * cameraSpeed * dt);
    }
    if (kbd.isKeyPressed('D')) {
        camera.adjustPosition(camera.getRightVector() * cameraSpeed * dt);
    }
    if (kbd.isKeyPressed(VK_SPACE)) {
        camera.adjustPosition(0.0f, cameraSpeed * dt, 0.0f);
    }
    if (kbd.isKeyPressed(VK_CONTROL)) {
        camera.adjustPosition(0.0f, - cameraSpeed * dt, 0.0f);
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
