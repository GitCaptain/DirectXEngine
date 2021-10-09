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
            if (me.getType() == HID::MouseEvent::EventType::RAW_MOVE) {
                //gfx.getCamera3D().adjustRotation(me.getPosY() * 0.01f, me.getPosX() * 0.01f, 0.0f);
            }
        }
    }
    
    // have to use it for directx::vector multiplications
    // it seems ugly, but its quick workaround for now
    using DirectX::operator*;
    using DirectX::operator+=;

    const float camera3DSpeed = 0.005f;
    float camera3DSpeedMultiplyer = 1.0f;

    if (keyboard.isKeyPressed(VK_SHIFT)) {
        //cameraSpeedMultiplyer *= 50;
        dt *= 50;
    }
    

    if (keyboard.isKeyPressed('W')) {
        //gfx.getCamera3D().adjustPosition(gfx.getCamera3D().getForwardVector() * camera3DSpeed * camera3DSpeedMultiplyer * dt);
    }
    if (keyboard.isKeyPressed('S')) {
        //gfx.getCamera3D().adjustPosition(gfx.getCamera3D().getBackwardVector() * camera3DSpeed * dt);
    }
    if (keyboard.isKeyPressed('A')) {
        //gfx.getCamera3D().adjustPosition(gfx.getCamera3D().getLeftVector() * camera3DSpeed * dt);
    }
    if (keyboard.isKeyPressed('D')) {
        //gfx.getCamera3D().adjustPosition(gfx.getCamera3D().getRightVector() * camera3DSpeed * dt);
    }
    if (keyboard.isKeyPressed(VK_SPACE)) {
        //gfx.getCamera3D().adjustPosition(0.0f, camera3DSpeed * dt, 0.0f);
    }
    if (keyboard.isKeyPressed(VK_CONTROL)) {
        //gfx.getCamera3D().adjustPosition(0.0f, - camera3DSpeed * dt, 0.0f);
    }

    if (keyboard.isKeyPressed('C')) {
        //DirectX::XMVECTOR lightPosition = gfx.getCamera3D().getPositionVector();
        //lightPosition += gfx.getCamera3D().getForwardVector();
        //gfx.light.setPosition(lightPosition);
        //gfx.light.setRotation(gfx.getCamera3D().getRotationFloat3());
    }
}

void Engine::renderFrame() {
    gfx.renderFrame();
}
