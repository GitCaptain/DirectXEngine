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
    gfx.update(keyboard, mouse, dt);
}

void Engine::renderFrame() {
    gfx.renderFrame();
}
