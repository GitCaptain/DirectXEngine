#include "Engine.h"

bool Engine::initialize(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height) {
    
    timer.startTimer();
    if (!renderWindow.initialize(this, hInstance, windowTitle, windowClass, width, height)) {
        return false;
    }
    if (!gfx.initialize(renderWindow.getHWND(), width, height)) {
        return false; 
    }
    return true;
}

bool Engine::processMessages() {
    return renderWindow.processMessages();
}

void Engine::Update() {
    float dt = timer.getMillisecondsElapsed();
    timer.restartTimer();

    while (!keyboard.isCharBufferEmpty()) {
        auto ch = keyboard.readChar();
    }

    while (!keyboard.isKeyBufferEmpty()) {
        auto event = keyboard.readKey();
        auto keycode = event.getKeyCode();
    }

    while (!mouse.isEventBufferEmpty()) {
        auto me = mouse.readEvent();
        if (mouse.isRightDown()) {
            if (me.getType() == Mouse::MouseEvent::EventType::RAW_MOVE) {
                gfx.getCamera().adjustRotation(me.getPosY() * 0.01f, me.getPosX() * 0.01f, 0.0f);
            }
        }
    }
    
    // have to use it for directx::vector multiplications
    // it seems ugly, but its quick workaround for now
    using DirectX::operator*;
    gfx.model.adjustRotation(0.0f, 0.001f * dt, 0.0f);
    const float cameraSpeed = 0.005f;
    float cameraSpeedMultiplyer = 1.0f;

    if (keyboard.isKeyPressed(VK_SHIFT)) {
        cameraSpeedMultiplyer *= 2;
    }
    

    if (keyboard.isKeyPressed('W')) {
        gfx.getCamera().adjustPosition(gfx.getCamera().getForwardVector() * cameraSpeed * cameraSpeedMultiplyer * dt);
    }
    if (keyboard.isKeyPressed('S')) {
        gfx.getCamera().adjustPosition(gfx.getCamera().getBackwardVector() * cameraSpeed * dt);
    }
    if (keyboard.isKeyPressed('A')) {
        gfx.getCamera().adjustPosition(gfx.getCamera().getLeftVector() * cameraSpeed * dt);
    }
    if (keyboard.isKeyPressed('D')) {
        gfx.getCamera().adjustPosition(gfx.getCamera().getRightVector() * cameraSpeed * dt);
    }
    if (keyboard.isKeyPressed(VK_SPACE)) {
        gfx.getCamera().adjustPosition(0.0f, cameraSpeed * dt, 0.0f);
    }
    if (keyboard.isKeyPressed(VK_CONTROL)) {
        gfx.getCamera().adjustPosition(0.0f, - cameraSpeed * dt, 0.0f);
    }
}

void Engine::renderFrame() {
    gfx.renderFrame();
}
