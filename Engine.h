#pragma once
#include "WindowContainer.h"
#include "Timer.h"

class Engine : WindowContainer {

public:
    Engine() = default;
    ~Engine() = default;
    bool initialize(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height);
    bool processMessages();  
    void Update();
    void renderFrame();

private:
    Timer timer;
};