#include "PongScene.h"
#include <string>
#include <format>
#include <iostream>

using namespace App;

bool PongScene::initialize(GraphicsState& graphicsState) {

    assert(graphicsState.device && "device is nullptr");
    assert(graphicsState.deviceContext && "deviceContext is nullptr");

    this->graphicsState = &graphicsState;

    windowWidth = graphicsState.windowWidth;
    windowHeight = graphicsState.windowHeight;

    if (!initializeConstantBuffers()) {
        return false;
    }

    if (!ball.initialize("Resources\\Objects\\wooden_sphere\\wooden_sphere.obj", graphicsState.device, graphicsState.deviceContext, cb_vs_vertexshader)) {
        return false;
    }
    if (!border.initialize("Resources\\Objects\\cube3d.fbx", graphicsState.device, graphicsState.deviceContext, cb_vs_vertexshader)) {
        return false;
    }

    spriteBatch = std::make_unique<DirectX::SpriteBatch>(graphicsState.deviceContext);
    spriteFont = std::make_unique<DirectX::SpriteFont>(graphicsState.device, L"Resources\\Fonts\\comic_sans_ms_16.spritefont");

    imgui = ImGUIWInstance::getPInstance();
    camera.setProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 3000.0f);
    reset();
    return true;
}

void PongScene::reset() {
    leftBorderPos = { -tableWidth / 2, 2 * tableHeight, 0 };
    rightBorderPos = { tableWidth / 2, 2 * tableHeight, 0 };

    PlayerPos = DefaultPlayerPos;
    AIPos = DefaultAIPos;

    if (gs.ballside == GameState::AI) {
        ballPosition = { AIPos.x, 2.f, tableLength / 2 - 3};
    }
    else {
        ballPosition = { PlayerPos.x, 2.f, -tableLength / 2 + 3};
    }

    playerSpeed = 0.3;
    AISpeed = 0.3;
    ballSpeed = 0;
    camera.setPosition(DefaultPlayerPos.x, tablePos.y + tableHeight + 50, DefaultPlayerPos.z - 20);
    camera.setLookAtPos((DefaultPlayerPos + tablePos)/2);
}

void PongScene::render() {

    cb_ps_phonglight.data.dynamicLightColor = light.lightColor;
    cb_ps_phonglight.data.dynamicLightStrength = light.lightStrength;
    cb_ps_phonglight.data.dynamicLightPosition = light.getPositionFloat3();
    cb_ps_phonglight.data.dynamicLightAttenuation_a = light.attenuation_a;
    cb_ps_phonglight.data.dynamicLightAttenuation_b = light.attenuation_b;
    cb_ps_phonglight.data.dynamicLightAttenuation_c = light.attenuation_c;

    //Should be configured from the material
    cb_ps_phonglight.data.shinessPower = 32;
    cb_ps_phonglight.data.specularStrength = 0.5;

    cb_ps_phonglight.applyChanges();
    graphicsState->deviceContext->PSSetConstantBuffers(0, 1, cb_ps_phonglight.GetAddressOf());

    cb_ps_camera.data.cameraWorldPosition = camera.getPositionFloat3();
    cb_ps_camera.applyChanges();
    graphicsState->deviceContext->PSSetConstantBuffers(1, 1, cb_ps_camera.GetAddressOf());

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

    // draw player pad
    border.setPosition(PlayerPos);
    border.setRotation(0, PI, 0);
    border.setScale(padWidth, padHeight, padLength);
    border.draw(viewProjectionMatrix);

    // draw AI pad
    border.setPosition(AIPos);
    border.setRotation(0, -PI , 0);
    border.setScale(padWidth, padHeight, padLength);
    border.draw(viewProjectionMatrix);

    // draw ball
    ball.setPosition(ballPosition);
    ball.setScale(ballRadius, ballRadius, ballRadius);
    ball.draw(viewProjectionMatrix);

    //light.draw(viewProjectionMatrix);

    // Score
    const std::string scores = std::format("Player {} : {} AI", gs.playerScore, gs.AIScore);
    spriteBatch->Begin();
    spriteFont->DrawString(spriteBatch.get(), scores.c_str(), DirectX::XMFLOAT2((windowWidth)/2 - scores.size(), 0), DirectX::Colors::Green, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
    spriteBatch->End();

#pragma region IMGUI drawing
#if 1
    imgui->startFrame();

    imgui->newWindow("Light controls")
        .attach<IMGUIFN::DRAGFLOAT3>("Ambient light color", &cb_ps_phonglight.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f)
        .attach<IMGUIFN::DRAGFLOAT>("Ambient light strength", &cb_ps_phonglight.data.ambientLightStrength, 0.01f, 0.0f, 1.0f)
        .attach<IMGUIFN::DRAGFLOAT>("Dynamic light Attenuation A", &light.attenuation_a, 0.01f, 0.1f, 1.0f)
        .attach<IMGUIFN::DRAGFLOAT>("Dynamic light Attenuation B", &light.attenuation_b, 0.01f, 0.0f, 1.0f)
        .attach<IMGUIFN::DRAGFLOAT>("Dynamic light Attenuation C", &light.attenuation_c, 0.01f, 0.0f, 1.0f)
        .end();

    imgui->newWindow("Camera controls")
        .attach<IMGUIFN::DRAGFLOAT>("Camera speed", &cameraSpeed, 0.005f, 0.0f, 0.1f)
        .attach<IMGUIFN::TEXT>("position: %.3f %.3f %.3f", camera.getPositionFloat3().x, camera.getPositionFloat3().y, camera.getPositionFloat3().z)
        .end();
    imgui->newWindow("table controls")
        //.attach<IMGUIFN::DRAGFLOAT3>("position", &tablePos.x, 1.f, 0.0f, 100.f)
        //.attach<IMGUIFN::DRAGFLOAT>("width", &tableWidth, 1.f, 0.0f, 100.f)
        //.attach<IMGUIFN::DRAGFLOAT>("height", &tableHeight, 1.f, 0.0f, 100.f)
        //.attach<IMGUIFN::DRAGFLOAT>("length", &tableLength, 1.f, 0.0f, 100.f)
        .end();
    imgui->newWindow("left border controls")
        //.attach<IMGUIFN::DRAGFLOAT3>("position", &leftBorderPos.x, 1.f, 0.0f, 100.f)
        //.attach<IMGUIFN::DRAGFLOAT>("width", &borderWidth, 1.f, 0.0f, 100.f)
        //.attach<IMGUIFN::DRAGFLOAT>("height", &borderHeight, 1.f, 0.0f, 100.f)
        //.attach<IMGUIFN::DRAGFLOAT>("length", &borderLength, 1.f, 0.0f, 100.f)
        .end();
    imgui->newWindow("right border controls")
        //.attach<IMGUIFN::DRAGFLOAT3>("position", &rightBorderPos.x, 1, 0.0f, 100.f)
        //.attach<IMGUIFN::DRAGFLOAT>("width", &borderWidth, 1.f, 0.0f, 100.f)
        //.attach<IMGUIFN::DRAGFLOAT>("height", &borderHeight, 1.f, 0.0f, 100.f)
        //.attach<IMGUIFN::DRAGFLOAT>("length", &borderLength, 1.f, 0.0f, 100.f)
        .end();
    imgui->newWindow("player pad controls")
        .attach<IMGUIFN::DRAGFLOAT3>("position", &PlayerPos.x, 1, -200.f, 200.f)
        .attach<IMGUIFN::DRAGFLOAT>("speed", &playerSpeed, 1, -200.f, 200.f)
        //.attach<IMGUIFN::DRAGFLOAT>("width", &padWidth, 1.f, 0.0f, 100.f)
        //.attach<IMGUIFN::DRAGFLOAT>("height", &padHeight, 1.f, 0.0f, 100.f)
        //.attach<IMGUIFN::DRAGFLOAT>("length", &padLength, 1.f, 0.0f, 100.f)
        .end();
    //imgui->newWindow("AI pad controls")
    //    .attach<IMGUIFN::DRAGFLOAT3>("position", &AIPos.x, 1, -200.f, 200.f)
    //    .attach<IMGUIFN::DRAGFLOAT>("width", &padWidth, 1.f, 0.0f, 100.f)
    //    .attach<IMGUIFN::DRAGFLOAT>("height", &padHeight, 1.f, 0.0f, 100.f)
    //    .attach<IMGUIFN::DRAGFLOAT>("length", &padLength, 1.f, 0.0f, 100.f)
    //    .end();
    imgui->newWindow("ball controls")
        .attach<IMGUIFN::DRAGFLOAT3>("position", &ballPosition.x, 1, -200.f, 200.f)
        //.attach<IMGUIFN::DRAGFLOAT>("radius", &ballRadius, 1.f, 0.0f, 100.f)
        .attach<IMGUIFN::DRAGFLOAT>("speed", &ballSpeed, 1.f, 0.0f, 100.f)
        .end();
    imgui->endFrame();
#endif
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
    if (kbd.isKeyPressed('Z')) {
        pushBall();
    }
    if (kbd.isKeyPressed('K')) {
        PlayerPos.x -= playerSpeed * dt;
        PlayerPos.x = std::max(PlayerPos.x, leftBorderPos.x + padWidth / 2);
    }
    if (kbd.isKeyPressed('L')) {
        PlayerPos.x += playerSpeed * dt;
        PlayerPos.x = std::min(PlayerPos.x, rightBorderPos.x - padWidth / 2);
    }
    if (kbd.isKeyPressed('R')) {
        reset();
    }
    if (kbd.isKeyPressed('C')) {
        DirectX::XMVECTOR lightPosition = camera.getPositionVector();
        lightPosition += camera.getForwardVector();
        light.setPosition(lightPosition);
        light.setRotation(camera.getRotationFloat3());
    }

    // If ball doesn't move, stick it to the pad
    if (ballSpeed == 0) {
        if (gs.ballside == GameState::PLAYER) {
            ballPosition.x = PlayerPos.x;
        }
        else {
            ballPosition.x = AIPos.x;
        }
    }
    // else update it's position
    else {
        ballPosition = ballPosition + ballDirection * (dt * ballSpeed);
    }

    updateAI(dt);
    checkCollision();
}

bool PongScene::initializeConstantBuffers() {

    // initialize constants buffers
    try {

        HRESULT hr = cb_vs_vertexshader.initialize(graphicsState->device, graphicsState->deviceContext);
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_vs_vertexhader constant buffer.");

        hr = cb_ps_phonglight.initialize(graphicsState->device, graphicsState->deviceContext);
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_ps_phonglight constant buffer.");

        hr = cb_ps_camera.initialize(graphicsState->device, graphicsState->deviceContext);
        COM_ERROR_IF_FAILED(hr, "Failed to initialize cb_ps_phonglight constant buffer.");
    }
    catch (const COMException& e) {
        ErrorLogger::log(e);
        return false;
    }

    cb_ps_phonglight.data.ambientLightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    cb_ps_phonglight.data.ambientLightStrength = 1.0f;

    if (!light.initialize(graphicsState->device, graphicsState->deviceContext, cb_vs_vertexshader)) {
        return false;
    }

    return true;
}

void App::PongScene::pushBall() {
    if (ballSpeed != 0) {
        return; // ball is moving already
    }
    ballSpeed = 0.0005;
    if (gs.ballside == GameState::PLAYER) {
        ballDirection = PlayerToAIDirection;
    }
    else {
        ballDirection = AIToPlayerDirection;
    }
}

void App::PongScene::updateAI(float dt) {
    // Move AI
    if (AIPos.x < ballPosition.x) {
        AIPos.x += dt * AISpeed;
    }
    else if (AIPos.x > ballPosition.x) {
        AIPos.x -= dt * AISpeed;
    }
    AIPos.x = std::max(AIPos.x, leftBorderPos.x + padWidth / 2);
    AIPos.x = std::min(AIPos.x, rightBorderPos.x - padWidth / 2);
}

void App::PongScene::checkCollision() {
    // Check for collisions
    if (ballPosition.x + ballRadius >= rightBorderPos.x || ballPosition.x - ballRadius <= leftBorderPos.x) {
        ballDirection.x *= -1;
    }

    // TODO: change these borders
    if (ballDirection.z < 0 && DefaultPlayerPos.z + ballRadius <= ballPosition.z && ballPosition.z <= DefaultPlayerPos.z + padLength / 2 + ballRadius) {
        if (PlayerPos.x - padWidth / 2 - ballRadius <= ballPosition.x && ballPosition.x <= PlayerPos.x + padWidth / 2 + ballRadius) {
            // in player window for shot
            ballDirection.z *= -1;
            ballDirection.x += (ballPosition.x - PlayerPos.x) * (PI/2/padWidth); // ((pi/4)/(width/2)) - max angle is 45
            std::cout << std::format("xdir: {}\n", ballDirection.x);
            return;
        }
    }
    if (ballDirection.z > 0 && DefaultAIPos.z - padLength / 2 <= ballPosition.z + ballRadius && ballPosition.z + ballRadius <= DefaultAIPos.z) {
        if (AIPos.x - padWidth / 2 - ballRadius <= ballPosition.x && ballPosition.x <= AIPos.x + padWidth / 2 + ballRadius) {
            //in ai window for shot
            ballDirection.z *= -1;
            ballDirection.x += (ballPosition.x - AIPos.x) * (PI / 2 / padWidth);
            std::cout << std::format("xdir: {}\n", ballDirection.x);
            return;
        }
    }

    // Check for goal
    if (ballPosition.z > DefaultAIPos.z) {
        gs.ballside = GameState::PLAYER;
        gs.playerScore += 1;
        reset();
    }

    if (ballPosition.z < DefaultPlayerPos.z) {
        gs.ballside = GameState::AI;
        gs.AIScore += 1;
        reset();
    }
}
