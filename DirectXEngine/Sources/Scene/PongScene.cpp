#include "PongScene.h"
#include <string>
#include <format>
#include <cstdlib>

using namespace App;

bool PongScene::initialize(GraphicsState& graphicsState, GraphicsSettings& graphicsSettings) {

    assert(graphicsState.device && "device is nullptr");
    assert(graphicsState.deviceContext && "deviceContext is nullptr");

    std::srand(std::time(nullptr));

    this->graphicsState = &graphicsState;
    this->graphicsSettings = &graphicsSettings;

    windowWidth = graphicsState.windowWidth;
    windowHeight = graphicsState.windowHeight;

    if (!ball.initialize("Resources\\Objects\\golf_ball\\10507_Golf Ball_v1_L3.obj", graphicsState.device)) {
        return false;
    }
    if (!leftBorder.initialize("Resources\\Objects\\cube3d.fbx", graphicsState.device)) {
        return false;
    }
    if (!rightBorder.initialize("Resources\\Objects\\cube3d.fbx", graphicsState.device)) {
        return false;
    }

    if (!table.initialize("Resources\\Objects\\pingpongtable\\10520_pingpongtable_L2.obj", graphicsState.device)) {
        return false;
    }
    if (!AIPad.initialize("Resources\\Objects\\Pingpong_paddle\\10519_Pingpong_paddle_v1_L3.obj", graphicsState.device)) {
        return false;
    }
    if (!playerPad.initialize("Resources\\Objects\\Pingpong_paddle\\10519_Pingpong_paddle_v1_L3.obj", graphicsState.device)) {
        return false;
    }

    p_renderables = { &ball, &leftBorder, &rightBorder, &table, &AIPad, &playerPad};
    constexpr int max_light_cnt = 6;
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
    updateProjectionSetting();
    reset();
    return true;
}

void PongScene::reset() {
    PlayerPos = DefaultPlayerPos;
    AIPos = DefaultAIPos;
    playerSpeed = defaultPadSpeed;
    AISpeed = defaultPadSpeed;
    ballSpeed = 0;
    if(gs.ballside == GameState::AI) {
        // Move AI pad to random x position on the table before pushing the ball
        const int randomXPos = std::rand();
        AITargetPosX = leftBorderPos.x + tableWidth * ( static_cast<float>(randomXPos) / RAND_MAX);
        AISpeed = defaultPadSpeed / 6;
    }

    float ballz = tableLength / 2 - 2.5 * ballRadius;
    if (gs.ballside == GameState::AI) {
        ballPosition = { AIPos.x, 2.f, ballz};
    }
    else { 
        ballPosition = { PlayerPos.x, 2.f, -ballz};
    }


    camera.setPosition(DefaultPlayerPos.x, tablePos.y + 50, DefaultPlayerPos.z - 20);
    camera.setLookAtPos((DefaultPlayerPos + tablePos)/2);
    timer.restartTimer();
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
        if (free_camera) {
            if (mouse.isRightDown()) {
                if (me.getType() == HID::MouseEvent::EventType::RAW_MOVE) {
                    camera.adjustRotation(me.getPosY() * 0.01f, me.getPosX() * 0.01f, 0.0f);
                }
            }
        }
    }

    if (free_camera) {
        float cameraSpeedMultiplyer = 1.0f;

        if (kbd.isKeyPressed(VK_SHIFT)) {
            cameraSpeedMultiplyer = 50.0f;
        }
        float camPosAdjMultiplier = cameraSpeed * cameraSpeedMultiplyer * dt;
        if (kbd.isKeyPressed('W')) {
            camera.adjustPosition(camera.getForwardVector() * camPosAdjMultiplier);
        }
        if (kbd.isKeyPressed('S')) {
            camera.adjustPosition(camera.getBackwardVector() * camPosAdjMultiplier);
        }
        if (kbd.isKeyPressed('A')) {
            camera.adjustPosition(camera.getLeftVector() * camPosAdjMultiplier);
        }
        if (kbd.isKeyPressed('D')) {
            camera.adjustPosition(camera.getRightVector() * camPosAdjMultiplier);
        }
        if (kbd.isKeyPressed(VK_SPACE)) {
            camera.adjustPosition(0.0f, camPosAdjMultiplier, 0.0f);
        }
        if (kbd.isKeyPressed(VK_CONTROL)) {
            camera.adjustPosition(0.0f, -camPosAdjMultiplier, 0.0f);
        }
    }
    if (kbd.isKeyPressed(VK_UP) && gs.ballside == GameState::PLAYER) {
        pushBall();
    }
    if (kbd.isKeyPressed(VK_LEFT)) {
        PlayerPos.x -= playerSpeed * dt;
        PlayerPos.x = std::max(PlayerPos.x, leftBorderPos.x + padWidth / 2);
    }
    if (kbd.isKeyPressed(VK_RIGHT)) {
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

    imgui->newWindow("game settings")
        .attach<IMGUIFN::CHECKBOX>("Free camera", &free_camera)
        .end();

#ifndef NDEBUG

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
    //imgui->newWindow("table controls")
    //    .attach<IMGUIFN::DRAGFLOAT3>("position", &tablePos.x, 1.f, 0.0f, 100.f)
    //    .attach<IMGUIFN::DRAGFLOAT>("width", &tableWidth, 1.f, 0.0f, 100.f)
    //    .attach<IMGUIFN::DRAGFLOAT>("height", &tableHeight, 1.f, 0.0f, 100.f)
    //    .attach<IMGUIFN::DRAGFLOAT>("length", &tableLength, 1.f, 0.0f, 100.f)
    //    .end();
    //imgui->newWindow("left border controls")
    //    .attach<IMGUIFN::DRAGFLOAT3>("position", &leftBorderPos.x, 1.f, 0.0f, 100.f)
    //    .attach<IMGUIFN::DRAGFLOAT>("width", &borderWidth, 1.f, 0.0f, 100.f)
    //    .attach<IMGUIFN::DRAGFLOAT>("height", &borderHeight, 1.f, 0.0f, 100.f)
    //    .attach<IMGUIFN::DRAGFLOAT>("length", &borderLength, 1.f, 0.0f, 100.f)
    //    .end();
    //imgui->newWindow("right border controls")
    //    .attach<IMGUIFN::DRAGFLOAT3>("position", &rightBorderPos.x, 1, 0.0f, 100.f)
    //    .attach<IMGUIFN::DRAGFLOAT>("width", &borderWidth, 1.f, 0.0f, 100.f)
    //    .attach<IMGUIFN::DRAGFLOAT>("height", &borderHeight, 1.f, 0.0f, 100.f)
    //    .attach<IMGUIFN::DRAGFLOAT>("length", &borderLength, 1.f, 0.0f, 100.f)
    //    .end();
    imgui->newWindow("player pad controls")
        .attach<IMGUIFN::DRAGFLOAT3>("position", &PlayerPos.x, 1, -200.f, 200.f)
    //    .attach<IMGUIFN::DRAGFLOAT>("speed", &playerSpeed, 1, -200.f, 200.f)
    //    .attach<IMGUIFN::DRAGFLOAT>("width", &padWidth, 1.f, 0.0f, 100.f)
    //    .attach<IMGUIFN::DRAGFLOAT>("height", &padHeight, 1.f, 0.0f, 100.f)
    //    .attach<IMGUIFN::DRAGFLOAT>("length", &padLength, 1.f, 0.0f, 100.f)
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
        .attach<IMGUIFN::DRAGFLOAT>("speed", &ballSpeed, 0.0001f, 0.0f, 100.f)
        .end();
#endif
#pragma endregion
    // Score
    const std::string scores = std::format("Player {} : {} AI", gs.playerScore, gs.AIScore);
    spriteBatch->Begin();
    spriteFont->DrawString(spriteBatch.get(), scores.c_str(), DirectX::XMFLOAT2((windowWidth) / 2 - scores.size(), 0), DirectX::Colors::Green, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
    spriteBatch->End();
}

void PongScene::updateGameObjects() {

    auto lowerPos = tablePos;
    lowerPos.y -= 100;
    table.setPosition(lowerPos);
    table.setRotation(PI/2, PI/2, PI/2);

    leftBorder.setPosition(leftBorderPos);
    leftBorder.setScale(borderWidth, borderHeight, borderLength);

    rightBorder.setPosition(rightBorderPos);
    rightBorder.setScale(borderWidth, borderHeight, borderLength);

    playerPad.setPosition(PlayerPos);
    playerPad.setRotation(PI/2, PI, 0);

    AIPad.setPosition(AIPos);
    AIPad.setRotation(PI/2, -PI, 0);

    ball.setPosition(ballPosition);
    ball.setScale(ballRadius, ballRadius, ballRadius);

    // if ball is in the game - it is AI target
    if (gs.ballside == GameState::NONE) {
        AITargetPosX = ballPosition.x;
    }
    for (int i = 0; i < light.getLightsCnt(); i++) {
        auto& pl = light.pointLights[i];
        auto& go_pos = p_renderables[i]->getPositionFloat3();
        pl.lightStrength = light.pointLights[0].lightStrength;
        pl.lightColor = light.pointLights[0].lightColor;
        pl.attenuations = light.pointLights[0].attenuations;
        pl.setPosition(go_pos);
    }
    updateProjectionSetting();

    if(gs.ballside == GameState::AI && timer.getMillisecondsElapsed() > AIWaitTime) {
        timer.stopTimer();
        AISpeed = defaultPadSpeed;
        pushBall();
    }
}

void PongScene::pushBall() {
    if (gs.ballside == GameState::NONE) {
        return; // ball is moving already
    }
    ballSpeed = defaultBallSpeed;
    if (gs.ballside == GameState::PLAYER) {
        ballDirection = PlayerToAIDirection;
    }
    else {
        ballDirection = AIToPlayerDirection;
    }
    gs.ballside = GameState::NONE;
}

void PongScene::updateAI(float dt) {
    // Move AI
    if (AIPos.x < AITargetPosX) {
        AIPos.x += dt * AISpeed;
    }
    else if (AIPos.x > AITargetPosX) {
        AIPos.x -= dt * AISpeed;
    }
    AIPos.x = std::max(AIPos.x, leftBorderPos.x + padWidth / 2);
    AIPos.x = std::min(AIPos.x, rightBorderPos.x - padWidth / 2);
}

void PongScene::checkCollision() {
    // Check for collisions with table borders
    if (ballPosition.x + ballRadWidth >= rightBorderPos.x || ballPosition.x - ballRadWidth <= leftBorderPos.x) {
        ballDirection.x *= -1;
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

    auto updateBall = [&](const XMFLOAT3& relPos) {
        ballDirection.z *= -1;
        const float reflectAngle = PI / 4 * (ballPosition.x - relPos.x) / maxXDiff; // max angle is 45 degrees
        if (reflectAngle == 0) {
            ballDirection.x = 0;
        }
        else {
            ballDirection.x = ballDirection.z / tan(reflectAngle); // z * ctg(ang), !! ang != 0 && ang != pi/2
            ballDirection.x = std::min(100.f, std::max(-100.f, ballDirection.x));
        }
        OutputDebugStringA(std::format("reflectAngle: {}, xdir: {}\n", reflectAngle, ballDirection.x).c_str());
        if (ballSpeed < maximumBallSpeed) {
            ballSpeed += deltaBallSpeed;
        }
    };

    // check if we are near the pad
    if(ballDirection.z == AIToPlayerDirection.z && DefaultPlayerPos.z + padLength/2 >= ballPosition.z - ballRadWidth) {
        // check if we are in pos to strike
        if(PlayerPos.x - maxXDiff <= ballPosition.x && ballPosition.x <= PlayerPos.x + maxXDiff) {
            updateBall(PlayerPos);
            return;
        }
    }

    // check if we are near the pad
    if (ballDirection.z == PlayerToAIDirection.z && DefaultAIPos.z - padLength / 2 <= ballPosition.z + ballRadWidth) {
        // check if we are in pos to strike
        if (AIPos.x - maxXDiff <= ballPosition.x && ballPosition.x <= AIPos.x + maxXDiff) {
            updateBall(AIPos);
            return;
        }
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
