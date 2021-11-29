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

    if (!ball.initialize("Resources\\Objects\\wooden_sphere\\wooden_sphere.obj", graphicsState.device)) {
        return false;
    }
    if (!leftBorder.initialize("Resources\\Objects\\cube3d.fbx", graphicsState.device)) {
        return false;
    }
    if (!rightBorder.initialize("Resources\\Objects\\cube3d.fbx", graphicsState.device)) {
        return false;
    }
    if (!table.initialize("Resources\\Objects\\cube3d.fbx", graphicsState.device)) {
        return false;
    }
    if (!AIPad.initialize("Resources\\Objects\\cube3d.fbx", graphicsState.device)) {
        return false;
    }
    if (!playerPad.initialize("Resources\\Objects\\cube3d.fbx", graphicsState.device)) {
        return false;
    }

    p_renderables = { &ball, &leftBorder, &rightBorder, &table, &AIPad, &playerPad};
    constexpr int max_light_cnt = 2;
    light.initialize(graphicsState, max_light_cnt);
    for (int i = 0; i < max_light_cnt; i++) {
        light.pointLights.emplace_back();
        if (!light.pointLights.back().initialize("Resources\\Objects\\light.fbx", graphicsState.device)) {
            return false;
        }
        p_renderables.push_back(&light.pointLights.back());
    }

    spriteBatch = std::make_unique<DirectX::SpriteBatch>(graphicsState.deviceContext);
    spriteFont = std::make_unique<DirectX::SpriteFont>(graphicsState.device, L"Resources\\Fonts\\comic_sans_ms_16.spritefont");

    imgui = ImGUIWInstance::getPInstance();
    camera.setProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 3000.0f);
    reset();
    timer.startTimer();
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

void PongScene::updateInput(HID::Keyboard& kbd, HID::Mouse& mouse, float dt) {

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
    if (kbd.isKeyPressed('B')) {
        camera.setPosition(ball.getPositionFloat3());
    }
    if (kbd.isKeyPressed('C')) {
        DirectX::XMVECTOR lightPosition = camera.getPositionVector();
        lightPosition += camera.getForwardVector();
        light.pointLights[0].setPosition(lightPosition);
        light.pointLights[0].setRotation(camera.getRotationFloat3());
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

}

void PongScene::updateGUI() {
#pragma region IMGUI drawing
#ifndef NDEBUG
    imgui->startFrame();

    imgui->newWindow("Light controls")
        .attach<IMGUIFN::DRAGFLOAT3>("Ambient light color", &light.ambient.lightColor.x, 0.01f, 0.0f, 1.0f)
        .attach<IMGUIFN::DRAGFLOAT>("Ambient light strength", &light.ambient.lightStrength, 0.01f, 0.0f, 1.0f)
        .attach<IMGUIFN::DRAGFLOAT3>("Dynamic light Attenuations", &light.pointLights[0].attenuations.x, 0.01f, 0.1f, 1.0f)
        .attach<IMGUIFN::DRAGFLOAT3>("Dynamic light Color", &light.pointLights[0].lightColor.x, 0.01f, 0.1f, 1.0f)
        .attach<IMGUIFN::DRAGFLOAT>("Dynamic light Strength", &light.pointLights[0].lightStrength, 0.01f, 0.1f, 1.0f)
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
    // Score
    const std::string scores = std::format("Player {} : {} AI", gs.playerScore, gs.AIScore);
    spriteBatch->Begin();
    spriteFont->DrawString(spriteBatch.get(), scores.c_str(), DirectX::XMFLOAT2((windowWidth) / 2 - scores.size(), 0), DirectX::Colors::Green, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
    spriteBatch->End();
}

void PongScene::updateGameObjects() {
    auto dt = timer.getMillisecondsElapsed();
    update_time += dt;
    timer.restartTimer();
    if (update_time > 50) {

        table.setRotation(0, 0, 0);
        table.setPosition(tablePos);
        table.setScale(tableWidth, tableHeight, tableLength);

        leftBorder.setPosition(leftBorderPos);
        leftBorder.setScale(borderWidth, borderHeight, borderLength);

        rightBorder.setPosition(rightBorderPos);
        rightBorder.setScale(borderWidth, borderHeight, borderLength);

        playerPad.setPosition(PlayerPos);
        playerPad.setRotation(0, PI, 0);
        playerPad.setScale(padWidth, padHeight, padLength);

        AIPad.setPosition(AIPos);
        AIPad.setRotation(0, -PI, 0);
        AIPad.setScale(padWidth, padHeight, padLength);

        ball.setPosition(ballPosition);
        ball.setScale(ballRadius, ballRadius, ballRadius);

        //for (int i = 0; i < light.getLightsCnt(); i++) {
        //    auto& pl = light.pointLights[i];
        //    auto& go = p_renderables[i]->getPositionFloat3();
        //    pl.setPosition(1000 * sin(dt), 1000 * cos(dt), 1000 * sin(update_time) * cos(update_time));
        //}

        light.pointLights[0].setPosition(ballPosition);
        light.pointLights[1].setPosition(PlayerPos);
    }
}

void PongScene::pushBall() {
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

void PongScene::updateAI(float dt) {
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

void PongScene::checkCollision() {
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

const LightInfo& PongScene::getLightInfo() const {
    return light;
}

const Camera3D& PongScene::getCameraInfo() const {
    return camera;
}

const XMMATRIX& PongScene::getViewMatrix() const {
    return camera.getViewMatrix();
}

const XMMATRIX& PongScene::getProjectionMatrix() const {
    return camera.getProjectionMatrix();
}
