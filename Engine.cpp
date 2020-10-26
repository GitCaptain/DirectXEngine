#include "Engine.h"

bool Engine::initialize(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height) {
    return renderWindow.initialize(this, hInstance, windowTitle, windowClass, width, height);
}

bool Engine::processMessages() {
    return renderWindow.processMessages();
}

void Engine::Update() {
    while (!keyboard.isCharBufferEmpty()) {
        auto ch = keyboard.readChar();
        std::string outmsg = "Char: ";
        outmsg += ch;
        outmsg += "\n";
        OutputDebugStringA(outmsg.c_str());
    }
    while (!keyboard.isKeyBufferEmpty()) {
        auto event = keyboard.readKey();
        auto keycode = event.getKeyCode();
        std::string outmsg;
        if (event.isPressEvent()) {
            outmsg = "key pressed: ";
        }
        else if (event.isReleaseEvent()) {
            outmsg = "key released: ";
        }
        else {
            outmsg = "unknown event: ";
        }
        outmsg += keycode;
        outmsg += "\n";
        OutputDebugStringA(outmsg.c_str());
    }

    while (!mouse.isEventBufferEmpty()) {
        auto me = mouse.readEvent();
        if (me.GetType() == Mouse::MouseEvent::EventType::RAW_MOVE) {
            std::string outmsg = "X: ";
            outmsg += std::to_string(me.GetPosX()) + ", Y: ";
            outmsg += std::to_string(me.GetPosY()) + "\n";
            OutputDebugStringA(outmsg.c_str());
        }
    }

}
