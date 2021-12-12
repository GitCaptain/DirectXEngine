#pragma once

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include "AdapterReader.h"
#include "GraphicsState.h"
#include "ImGUIWrapper.h"
#include "../Timer.h"
#include "../Scene/PongScene.h"
#include "Render/DeferredRenderer.h"
#include "Render/ForwardRenderer.h"

class Graphics {
public:
    bool initialize(HWND hwnd, int width, int height);
    void renderFrame();
    void update(HID::Keyboard& kbd, HID::Mouse& mouse, float dt);
    void postRender();
    void present();
    GraphicsState& getGraphicsState() const;

private:
    bool initializeRenderer(HWND hwnd, int width, int height);
    bool initializeScene();
    void initSprites();

    //std::unique_ptr<Renderer> renderer{ new ForwardRenderer{} };
    std::unique_ptr<Renderer> renderer{ new DeferredRenderer{} };
    std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
    std::unique_ptr<DirectX::SpriteFont> spriteFont;

    Timer fpsTimer;
    std::unique_ptr<App::Scene> renderScene{new App::PongScene{}};
    ImGUIW *imgui = nullptr;

    GraphicsSettings gSettings;
    bool showfps = false;
};
